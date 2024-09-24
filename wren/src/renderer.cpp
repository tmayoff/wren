#include "wren/renderer.hpp"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <wren_vk/errors.hpp>

#include "utils/tracy.hpp"  // IWYU pragma: export
#include "wren/context.hpp"
#include "wren/mesh.hpp"
#include "wren/render_pass.hpp"

namespace wren {

void Renderer::draw() {
  ZoneScoped;
  auto res = begin_frame();
  if (!res.has_value()) return;
  end_frame(res.value());
}

auto Renderer::begin_frame() -> expected<uint32_t> {
  ZoneScoped;

  ::vk::Result res = ::vk::Result::eSuccess;

  auto const &device = ctx_->graphics_context->Device().get();
  {
    ZoneScopedN("device.waitForFences()");  // NOLINT
    VK_ERR_PROP_VOID(
        device.waitForFences(in_flight_fence, VK_TRUE, UINT64_MAX));
    device.resetFences(in_flight_fence);
  }

  uint32_t image_index = -1;

  {
    ZoneScopedN("device.acquireNextImageKHR()");
    std::tie(res, image_index) = device.acquireNextImageKHR(
        swapchain, UINT64_MAX, image_available);
    if (res == ::vk::Result::eErrorOutOfDateKHR) {
      recreate_swapchain();
      return tl::make_unexpected(make_error_code(res));
    }
  }

  render_targets_.at(SWAPCHAIN_RENDERTARGET_NAME.data())->image_view =
      swapchain_image_views_.at(image_index);

  return image_index;
}

void Renderer::end_frame(uint32_t image_index) {
  ::vk::PipelineStageFlags waitDstStageMask =
      ::vk::PipelineStageFlagBits::eColorAttachmentOutput;

  std::vector<::vk::CommandBuffer> cmd_bufs;
  for (auto g : render_graph) {
    ZoneScopedN("render_pass->execute()");
    g->render_pass->execute();
    cmd_bufs = g->render_pass->get_command_buffers();
  }

  ::vk::SubmitInfo submit_info(image_available, waitDstStageMask,
                               cmd_bufs, render_finished);
  ::vk::Result res =
      ctx_->graphics_context->Device().get_graphics_queue().submit(
          submit_info, in_flight_fence);
  if (res != ::vk::Result::eSuccess) {
    spdlog::warn("{}", ::vk::to_string(res));
  }

  ::vk::PresentInfoKHR present_info{render_finished, swapchain,
                                    image_index};
  res =
      ctx_->graphics_context->Device().get_present_queue().presentKHR(
          present_info);
  if (res == ::vk::Result::eErrorOutOfDateKHR ||
      res == ::vk::Result::eSuboptimalKHR) {
    recreate_swapchain();
    return;
  }

  if (res != ::vk::Result::eSuccess) {
    spdlog::warn("{}", ::vk::to_string(res));
  }
}

auto Renderer::submit_command_buffer(
    std::function<void(::vk::CommandBuffer &)> const &cmd_buf)
    -> expected<void> {
  if (!cmd_buf) return {};

  VK_CHECK_RESULT(
      one_time_cmd_buffer.begin(::vk::CommandBufferBeginInfo(
          ::vk::CommandBufferUsageFlagBits::eOneTimeSubmit)));

  cmd_buf(one_time_cmd_buffer);

  VK_CHECK_RESULT(one_time_cmd_buffer.end());

  ::vk::SubmitInfo submit_info({}, {}, one_time_cmd_buffer);
  VK_CHECK_RESULT(
      ctx_->graphics_context->Device().get_graphics_queue().submit(
          submit_info));

  VK_CHECK_RESULT(ctx_->graphics_context->Device()
                      .get_graphics_queue()
                      .waitIdle());

  return {};
}

Renderer::Renderer(std::shared_ptr<Context> const &ctx)
    : ctx_(ctx),
      m(ctx->graphics_context->Device(),
        ctx->graphics_context->allocator()) {
  ctx->event_dispatcher.on<Event::WindowResized>(
      [this](auto &w) { recreate_swapchain(); });
}

auto Renderer::New(std::shared_ptr<Context> const &ctx)
    -> expected<std::shared_ptr<Renderer>> {
  ZoneScoped;

  auto device = ctx->graphics_context->Device();

  auto renderer = std::shared_ptr<Renderer>(new Renderer(ctx));
  ctx->renderer = renderer;

  auto res = renderer->recreate_swapchain();
  if (!res.has_value()) return tl::make_unexpected(res.error());

  ::vk::Result vres = ::vk::Result::eSuccess;
  std::tie(vres, renderer->in_flight_fence) =
      device.get().createFence(::vk::FenceCreateInfo{
          ::vk::FenceCreateFlagBits::eSignaled});
  if (vres != ::vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(vres));

  std::tie(vres, renderer->image_available) =
      device.get().createSemaphore(::vk::SemaphoreCreateInfo{});
  if (vres != ::vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(vres));

  std::tie(vres, renderer->render_finished) =
      device.get().createSemaphore(::vk::SemaphoreCreateInfo{});
  if (vres != ::vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(vres));

  VK_TIE_RESULT(
      renderer->command_pool_,
      device.get().createCommandPool(::vk::CommandPoolCreateInfo{
          ::vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
          ctx->graphics_context->FindQueueFamilyIndices()
              .value()
              .graphics_index}));

  ::vk::CommandBufferAllocateInfo alloc_info(
      renderer->command_pool_, ::vk::CommandBufferLevel::ePrimary, 1);

  VK_TRY_RESULT(
      bufs,
      ctx->graphics_context->Device().get().allocateCommandBuffers(
          alloc_info));

  renderer->one_time_cmd_buffer = bufs.front();

  return renderer;
}

auto Renderer::recreate_swapchain() -> expected<void> {
  ZoneScoped;  // NOLINT
  ::vk::Result res = ::vk::Result::eSuccess;

  auto const &device = ctx_->graphics_context->Device();

  {
    ZoneScopedN(
        "ctx->graphics_context->Device().get().waitIdle()");  // NOLINT
    res = ctx_->graphics_context->Device().get().waitIdle();
  }

  // ============ Destroy previous resources
  // for (auto const &node : render_graph) {
  // auto const &fbs = node->render_pass->get_framebuffers();
  // for (auto const &fb : fbs) device.get().destroyFramebuffer(fb);
  // }

  if (render_targets_.contains(SWAPCHAIN_RENDERTARGET_NAME.data())) {
    for (auto const &iv : swapchain_image_views_)
      device.get().destroyImageView(iv);
    swapchain_image_views_.clear();
  }

  device.get().destroySwapchainKHR(swapchain);

  //=========== Create Swapchain
  auto swapchain_support =
      ctx_->graphics_context->GetSwapchainSupport();
  if (!swapchain_support.has_value())
    return tl::make_unexpected(swapchain_support.error());

  auto format =
      choose_swapchain_format(swapchain_support->surface_formats);
  auto present_mode = choose_swapchain_presentation_mode(
      swapchain_support->present_modes);
  swapchain_extent =
      choose_swapchain_extent(swapchain_support->surface_capabilites);

  auto image_count =
      swapchain_support->surface_capabilites.minImageCount + 1;
  if (swapchain_support->surface_capabilites.maxImageCount > 0 &&
      image_count >
          swapchain_support->surface_capabilites.maxImageCount) {
    image_count =
        swapchain_support->surface_capabilites.maxImageCount;
  }

  ::vk::SwapchainCreateInfoKHR create_info(
      {}, ctx_->graphics_context->Surface(), image_count,
      format.format, format.colorSpace, swapchain_extent, 1,
      ::vk::ImageUsageFlagBits::eColorAttachment);

  auto queue_families = vulkan::Queue::FindQueueFamilyIndices(
      ctx_->graphics_context->PhysicalDevice(),
      ctx_->graphics_context->Surface());
  if (!queue_families.has_value())
    return tl::make_unexpected(queue_families.error());

  std::array queue_indices = {queue_families->graphics_index,
                              queue_families->present_index};
  if (queue_families->graphics_index !=
      queue_families->present_index) {
    create_info.setImageSharingMode(::vk::SharingMode::eConcurrent);
    create_info.setQueueFamilyIndices(queue_indices);
  } else {
    create_info.setImageSharingMode(::vk::SharingMode::eExclusive);
  }

  create_info.setPreTransform(
      swapchain_support->surface_capabilites.currentTransform);
  create_info.setCompositeAlpha(
      ::vk::CompositeAlphaFlagBitsKHR::eOpaque);
  create_info.setPresentMode(present_mode);
  create_info.setClipped(true);

  std::tie(res, swapchain) =
      ctx_->graphics_context->Device().get().createSwapchainKHR(
          create_info);
  if (res != ::vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::tie(res, swapchain_images) =
      ctx_->graphics_context->Device().get().getSwapchainImagesKHR(
          swapchain);
  if (res != ::vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  swapchain_image_format = format.format;

  swapchain_image_views_.reserve(swapchain_images.size());
  for (auto const &swapchain_image : swapchain_images) {
    ::vk::ImageViewCreateInfo create_info(
        {}, swapchain_image, ::vk::ImageViewType::e2D,
        swapchain_image_format, {},
        ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor,
                                    0, 1, 0, 1));

    ::vk::ImageView image_view;
    VK_TIE_ERR_PROP(
        image_view,
        ctx_->graphics_context->Device().get().createImageView(
            create_info));

    swapchain_image_views_.push_back(image_view);
  }

  if (!render_targets_.contains(SWAPCHAIN_RENDERTARGET_NAME.data())) {
    render_targets_.emplace(
        SWAPCHAIN_RENDERTARGET_NAME.data(),
        std::make_shared<RenderTarget>(
            swapchain_extent, swapchain_image_format,
            ::vk::SampleCountFlagBits::e1,
            swapchain_image_views_.front(),
            ::vk::ImageUsageFlagBits::eColorAttachment));
  } else {
    auto const &target =
        render_targets_.at(SWAPCHAIN_RENDERTARGET_NAME.data());
    target->size = swapchain_extent;
    target->format = swapchain_image_format;
    target->image_view = swapchain_image_views_.front();
  }

  for (auto const &g : render_graph)
    g->render_pass->recreate_framebuffers(
        ctx_->graphics_context->Device().get());

  return {};
}

auto Renderer::choose_swapchain_format(
    std::vector<::vk::SurfaceFormatKHR> const &formats)
    -> ::vk::SurfaceFormatKHR {
  auto const PREFERED_FORMAT = ::vk::Format::eB8G8R8Srgb;
  auto const PREFERED_COLOR_SPACE =
      ::vk::ColorSpaceKHR::eSrgbNonlinear;

  for (auto const &format : formats) {
    if (format.format == PREFERED_FORMAT &&
        format.colorSpace == PREFERED_COLOR_SPACE)
      return format;
  }

  return formats.front();
}

auto Renderer::choose_swapchain_presentation_mode(
    std::vector<::vk::PresentModeKHR> const &modes)
    -> ::vk::PresentModeKHR {
  auto const PREFERED_PRESENT_MDOE = ::vk::PresentModeKHR::eMailbox;

  for (auto const &mode : modes) {
    if (mode == PREFERED_PRESENT_MDOE) return mode;
  }

  return ::vk::PresentModeKHR::eFifo;
}

auto Renderer::choose_swapchain_extent(
    ::vk::SurfaceCapabilitiesKHR const &surface_capabilities)
    -> ::vk::Extent2D {
  if (surface_capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  } else {
    auto [width, height] = ctx_->window.GetSize();
    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(
        actualExtent.width, surface_capabilities.minImageExtent.width,
        surface_capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height,
                   surface_capabilities.minImageExtent.height,
                   surface_capabilities.maxImageExtent.height);

    return actualExtent;
  }
}
}  // namespace wren
