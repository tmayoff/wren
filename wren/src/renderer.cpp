#include "wren/renderer.hpp"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <wren/vk/result.hpp>

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

  const auto &device = ctx_->graphics_context->Device().get();
  {
    ZoneScopedN("device.waitForFences()");  // NOLINT
    VK_ERR_PROP_VOID(
        device.waitForFences(in_flight_fence, VK_TRUE, UINT64_MAX));
    device.resetFences(in_flight_fence);
  }

  uint32_t image_index = -1;

  {
    ZoneScopedN("device.acquireNextImageKHR()");
    std::tie(res, image_index) =
        device.acquireNextImageKHR(swapchain_, UINT64_MAX, image_available);
    if (res == ::vk::Result::eErrorOutOfDateKHR) {
      recreate_swapchain();
      return std::unexpected(make_error_code(res));
    }
  }

  render_targets_.at(SWAPCHAIN_RENDERTARGET_NAME.data())->image_view =
      swapchain_image_views_.at(image_index);

  return image_index;
}

void Renderer::end_frame(uint32_t image_index) {
  ::vk::PipelineStageFlags wait_dst_stage_mask =
      ::vk::PipelineStageFlagBits::eColorAttachmentOutput;

  std::vector<::vk::CommandBuffer> cmd_bufs;
  for (auto g : render_graph) {
    ZoneScopedN("render_pass->execute()");
    g->render_pass->execute();
    const auto bufs = g->render_pass->get_command_buffers();
    cmd_bufs.insert(cmd_bufs.end(), bufs.begin(), bufs.end());
  }

  ::vk::SubmitInfo submit_info(image_available, wait_dst_stage_mask, cmd_bufs,
                               render_finished);
  ::vk::Result res =
      ctx_->graphics_context->Device().get_graphics_queue().submit(
          submit_info, in_flight_fence);
  if (res != ::vk::Result::eSuccess) {
    spdlog::warn("{}", ::vk::to_string(res));
  }

  ::vk::PresentInfoKHR present_info{render_finished, swapchain_, image_index};
  res = ctx_->graphics_context->Device().get_present_queue().presentKHR(
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
    const std::function<void(::vk::CommandBuffer &)> &cmd_buf)
    -> expected<void> {
  if (!cmd_buf) return {};

  VK_CHECK_RESULT(one_time_cmd_buffer.begin(::vk::CommandBufferBeginInfo(
      ::vk::CommandBufferUsageFlagBits::eOneTimeSubmit)));

  cmd_buf(one_time_cmd_buffer);

  VK_CHECK_RESULT(one_time_cmd_buffer.end());

  ::vk::SubmitInfo submit_info({}, {}, one_time_cmd_buffer);
  VK_CHECK_RESULT(ctx_->graphics_context->Device().get_graphics_queue().submit(
      submit_info));

  VK_CHECK_RESULT(
      ctx_->graphics_context->Device().get_graphics_queue().waitIdle());

  return {};
}

Renderer::Renderer(const std::shared_ptr<Context> &ctx)
    : ctx_(ctx),
      m(ctx->graphics_context->Device(), ctx->graphics_context->allocator()) {
  ctx->event_dispatcher.on<event::WindowResized>(
      [this](auto &w) { recreate_swapchain(); });
}

auto Renderer::create(const std::shared_ptr<Context> &ctx)
    -> expected<std::shared_ptr<Renderer>> {
  ZoneScoped;

  const auto device = ctx->graphics_context->Device();

  auto renderer = std::shared_ptr<Renderer>(new Renderer(ctx));
  ctx->renderer = renderer;

  auto res = renderer->recreate_swapchain();
  if (!res.has_value()) return std::unexpected(res.error());

  ::vk::Result vres = ::vk::Result::eSuccess;
  std::tie(vres, renderer->in_flight_fence) = device.get().createFence(
      ::vk::FenceCreateInfo{::vk::FenceCreateFlagBits::eSignaled});
  if (vres != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(vres));

  std::tie(vres, renderer->image_available) =
      device.get().createSemaphore(::vk::SemaphoreCreateInfo{});
  if (vres != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(vres));

  std::tie(vres, renderer->render_finished) =
      device.get().createSemaphore(::vk::SemaphoreCreateInfo{});
  if (vres != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(vres));

  VK_TIE_RESULT(renderer->command_pool_,
                device.get().createCommandPool(::vk::CommandPoolCreateInfo{
                    ::vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                    ctx->graphics_context->FindQueueFamilyIndices()
                        .value()
                        .graphics_index}));

  ::vk::CommandBufferAllocateInfo alloc_info(
      renderer->command_pool_, ::vk::CommandBufferLevel::ePrimary, 1);

  VK_TRY_RESULT(
      bufs,
      ctx->graphics_context->Device().get().allocateCommandBuffers(alloc_info));

  renderer->one_time_cmd_buffer = bufs.front();

  return renderer;
}

auto Renderer::recreate_swapchain() -> expected<void> {
  ZoneScoped;  // NOLINT
  ::vk::Result res = ::vk::Result::eSuccess;

  const auto &device = ctx_->graphics_context->Device();

  {
    ZoneScopedN("ctx->graphics_context->Device().get().waitIdle()");  // NOLINT
    res = ctx_->graphics_context->Device().get().waitIdle();
  }

  // ============ Destroy previous resources
  // for (auto const &node : render_graph) {
  // auto const &fbs = node->render_pass->get_framebuffers();
  // for (auto const &fb : fbs) device.get().destroyFramebuffer(fb);
  // }

  if (render_targets_.contains(SWAPCHAIN_RENDERTARGET_NAME.data())) {
    for (const auto &iv : swapchain_image_views_)
      device.get().destroyImageView(iv);
    swapchain_image_views_.clear();
  }

  device.get().destroySwapchainKHR(swapchain_);

  //=========== Create Swapchain
  auto swapchain_support = ctx_->graphics_context->GetSwapchainSupport();
  if (!swapchain_support.has_value())
    return std::unexpected(swapchain_support.error());

  auto format = choose_swapchain_format(swapchain_support->surface_formats);
  auto present_mode =
      choose_swapchain_presentation_mode(swapchain_support->present_modes);
  swapchain_extent =
      choose_swapchain_extent(swapchain_support->surface_capabilites);

  auto image_count = swapchain_support->surface_capabilites.minImageCount + 1;
  if (swapchain_support->surface_capabilites.maxImageCount > 0 &&
      image_count > swapchain_support->surface_capabilites.maxImageCount) {
    image_count = swapchain_support->surface_capabilites.maxImageCount;
  }

  ::vk::SwapchainCreateInfoKHR create_info(
      {}, ctx_->graphics_context->Surface(), image_count, format.format,
      format.colorSpace, swapchain_extent, 1,
      ::vk::ImageUsageFlagBits::eColorAttachment);

  auto queue_families = vulkan::Queue::find_queue_family_indices(
      ctx_->graphics_context->PhysicalDevice(),
      ctx_->graphics_context->Surface());
  if (!queue_families.has_value())
    return std::unexpected(queue_families.error());

  std::array queue_indices = {queue_families->graphics_index,
                              queue_families->present_index};
  if (queue_families->graphics_index != queue_families->present_index) {
    create_info.setImageSharingMode(::vk::SharingMode::eConcurrent);
    create_info.setQueueFamilyIndices(queue_indices);
  } else {
    create_info.setImageSharingMode(::vk::SharingMode::eExclusive);
  }

  create_info.setPreTransform(
      swapchain_support->surface_capabilites.currentTransform);
  create_info.setCompositeAlpha(::vk::CompositeAlphaFlagBitsKHR::eOpaque);
  create_info.setPresentMode(present_mode);
  create_info.setClipped(true);

  std::tie(res, swapchain_) =
      ctx_->graphics_context->Device().get().createSwapchainKHR(create_info);
  if (res != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(res));

  std::tie(res, swapchain_images_) =
      ctx_->graphics_context->Device().get().getSwapchainImagesKHR(swapchain_);
  if (res != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(res));

  swapchain_image_format = format.format;

  swapchain_image_views_.reserve(swapchain_images_.size());
  for (const auto &swapchain_image : swapchain_images_) {
    ::vk::ImageViewCreateInfo create_info(
        {}, swapchain_image, ::vk::ImageViewType::e2D, swapchain_image_format,
        {},
        ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1, 0,
                                    1));

    ::vk::ImageView image_view;
    VK_TIE_ERR_PROP(
        image_view,
        ctx_->graphics_context->Device().get().createImageView(create_info));

    swapchain_image_views_.push_back(image_view);
  }

  if (!render_targets_.contains(SWAPCHAIN_RENDERTARGET_NAME.data())) {
    render_targets_.emplace(
        SWAPCHAIN_RENDERTARGET_NAME.data(),
        std::make_shared<RenderTarget>(
            math::vec2i{static_cast<int32_t>(swapchain_extent.width),
                        static_cast<int32_t>(swapchain_extent.height)},
            swapchain_image_format, ::vk::SampleCountFlagBits::e1,
            swapchain_image_views_.front(),
            ::vk::ImageUsageFlagBits::eColorAttachment));
  } else {
    const auto &target = render_targets_.at(SWAPCHAIN_RENDERTARGET_NAME.data());
    target->size = {static_cast<int32_t>(swapchain_extent.width),
                    static_cast<int32_t>(swapchain_extent.height)};
    target->format = swapchain_image_format;
    target->image_view = swapchain_image_views_.front();
  }

  for (const auto &g : render_graph)
    g->render_pass->recreate_framebuffers(
        ctx_->graphics_context->Device().get());

  return {};
}

auto Renderer::choose_swapchain_format(
    const std::vector<::vk::SurfaceFormatKHR> &formats)
    -> ::vk::SurfaceFormatKHR {
  const auto prefered_format = ::vk::Format::eB8G8R8Srgb;
  const auto prefered_color_space = ::vk::ColorSpaceKHR::eSrgbNonlinear;

  for (const auto &format : formats) {
    if (format.format == prefered_format &&
        format.colorSpace == prefered_color_space)
      return format;
  }

  return formats.front();
}

auto Renderer::choose_swapchain_presentation_mode(
    const std::vector<::vk::PresentModeKHR> &modes) -> ::vk::PresentModeKHR {
  const auto prefered_present_mdoe = ::vk::PresentModeKHR::eMailbox;

  for (const auto &mode : modes) {
    if (mode == prefered_present_mdoe) return mode;
  }

  return ::vk::PresentModeKHR::eFifo;
}

auto Renderer::choose_swapchain_extent(
    const ::vk::SurfaceCapabilitiesKHR &surface_capabilities)
    -> ::vk::Extent2D {
  if (surface_capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  } else {
    const auto [width, height] = ctx_->window.get_size();
    VkExtent2D actual_extent = {static_cast<uint32_t>(width),
                                static_cast<uint32_t>(height)};

    actual_extent.width = std::clamp(actual_extent.width,
                                     surface_capabilities.minImageExtent.width,
                                     surface_capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(
        actual_extent.height, surface_capabilities.minImageExtent.height,
        surface_capabilities.maxImageExtent.height);

    return actual_extent;
  }
}
}  // namespace wren
