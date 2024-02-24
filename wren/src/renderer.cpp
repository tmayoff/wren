#include "wren/renderer.hpp"

#include <Tracy/tracy/Tracy.hpp>
#include <cstdint>
#include <gsl/gsl-lite.hpp>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "wren/context.hpp"
#include "wren/graph.hpp"
#include "wren/shader.hpp"
#include "wren/shaders/triangle.hpp"
#include "wren/utils/queue.hpp"
#include "wren/utils/vulkan.hpp"

namespace wren {

void Renderer::draw() {
  ZoneScoped;
  begin_frame();

  end_frame();
}

void Renderer::begin_frame() {
  ZoneScoped;

  const auto &device = ctx->graphics_context.Device().get();

  device.resetFences(in_flight_fence);
  auto res =
      device.waitForFences(in_flight_fence, VK_TRUE, UINT64_MAX);
  // TODO Error handling
  device.resetFences(in_flight_fence);

  uint32_t image_index = -1;
  std::tie(res, image_index) = device.acquireNextImageKHR(
      swapchain, UINT64_MAX, image_available);

  for (auto g : render_graph) {
    g->render_pass->execute();

    vk::PipelineStageFlags waitDstStageMask =
        vk::PipelineStageFlagBits::eTopOfPipe;
    vk::SubmitInfo submit_info(
        image_available, waitDstStageMask,
        g->render_pass->get_command_buffers().front(),
        render_finished);
    auto res =
        ctx->graphics_context.Device().get_graphics_queue().submit(
            submit_info, in_flight_fence);

    vk::PresentInfoKHR present_info{render_finished, swapchain,
                                    image_index};
    res =
        ctx->graphics_context.Device().get_present_queue().presentKHR(
            present_info);
  }
}

void Renderer::end_frame() {}

auto Renderer::Create(const std::shared_ptr<Context> &ctx)
    -> tl::expected<std::shared_ptr<Renderer>, std::error_code> {
  ZoneScoped;
  auto renderer = std::shared_ptr<Renderer>(new Renderer(ctx));
  ctx->renderer = renderer;

  auto res = renderer->create_swapchain();
  if (!res.has_value()) return tl::make_unexpected(res.error());

  auto shader = Shader::Create(ctx->graphics_context.Device(),
                               TRIANGLE_VERT_SHADER.data(),
                               TRIANGLE_FRAG_SHADER.data());

  renderer->build_3D_render_graph();

  vk::Result vres = vk::Result::eSuccess;
  std::tie(vres, renderer->in_flight_fence) =
      ctx->graphics_context.Device().get().createFence(
          vk::FenceCreateInfo{});

  return renderer;
}

auto Renderer::create_swapchain()
    -> tl::expected<void, std::error_code> {
  auto swapchain_support =
      ctx->graphics_context.GetSwapchainSupport();
  if (!swapchain_support.has_value())
    return tl::make_unexpected(swapchain_support.error());

  auto format =
      choose_swapchain_format(swapchain_support->surface_formats);
  auto present_mode = choose_swapchain_presentation_mode(
      swapchain_support->present_modes);
  auto extent =
      choose_swapchain_extent(swapchain_support->surface_capabilites);

  auto image_count =
      swapchain_support->surface_capabilites.minImageCount + 1;
  if (swapchain_support->surface_capabilites.maxImageCount > 0 &&
      image_count >
          swapchain_support->surface_capabilites.maxImageCount) {
    image_count =
        swapchain_support->surface_capabilites.maxImageCount;
  }

  vk::SwapchainCreateInfoKHR create_info(
      {}, ctx->graphics_context.Surface(), image_count, format.format,
      format.colorSpace, extent, 1,
      vk::ImageUsageFlagBits::eColorAttachment);

  auto queue_families = vulkan::Queue::FindQueueFamilyIndices(
      ctx->graphics_context.PhysicalDevice(),
      ctx->graphics_context.Surface());
  if (!queue_families.has_value())
    return tl::make_unexpected(queue_families.error());

  std::array queue_indices = {queue_families->graphics_index,
                              queue_families->present_index};
  if (queue_families->graphics_index !=
      queue_families->present_index) {
    create_info.setImageSharingMode(vk::SharingMode::eConcurrent);
    create_info.setQueueFamilyIndices(queue_indices);
  } else {
    create_info.setImageSharingMode(vk::SharingMode::eExclusive);
  }

  create_info.setPreTransform(
      swapchain_support->surface_capabilites.currentTransform);
  create_info.setCompositeAlpha(
      vk::CompositeAlphaFlagBitsKHR::eOpaque);
  create_info.setPresentMode(present_mode);
  create_info.setClipped(true);

  vk::Result res = vk::Result::eSuccess;
  std::tie(res, swapchain) =
      ctx->graphics_context.Device().get().createSwapchainKHR(
          create_info);
  if (res != vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::tie(res, swapchain_images) =
      ctx->graphics_context.Device().get().getSwapchainImagesKHR(
          swapchain);
  if (res != vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  swapchain_image_format = format.format;
  swapchain_extent = extent;

  swapchain_image_views.resize(swapchain_images.size());
  for (size_t i = 0; i < swapchain_images.size(); i++) {
    vk::ImageViewCreateInfo create_info(
        {}, swapchain_images[i], vk::ImageViewType::e2D,
        swapchain_image_format, {},
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0,
                                  1, 0, 1));

    std::tie(res, swapchain_image_views[i]) =
        ctx->graphics_context.Device().get().createImageView(
            create_info);
    if (res != vk::Result::eSuccess)
      return tl::make_unexpected(make_error_code(res));
  }

  return {};
}

auto Renderer::choose_swapchain_format(
    const std::vector<vk::SurfaceFormatKHR> &formats)
    -> vk::SurfaceFormatKHR {
  const auto PREFERED_FORMAT = vk::Format::eB8G8R8Srgb;
  const auto PREFERED_COLOR_SPACE = vk::ColorSpaceKHR::eSrgbNonlinear;

  for (const auto &format : formats) {
    if (format.format == PREFERED_FORMAT &&
        format.colorSpace == PREFERED_COLOR_SPACE)
      return format;
  }

  return formats.front();
}

auto Renderer::choose_swapchain_presentation_mode(
    const std::vector<vk::PresentModeKHR> &modes)
    -> vk::PresentModeKHR {
  const auto PREFERED_PRESENT_MDOE = vk::PresentModeKHR::eMailbox;

  for (const auto &mode : modes) {
    if (mode == PREFERED_PRESENT_MDOE) return mode;
  }

  return vk::PresentModeKHR::eFifo;
}

auto Renderer::choose_swapchain_extent(
    const vk::SurfaceCapabilitiesKHR &surface_capabilities)
    -> vk::Extent2D {
  if (surface_capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  } else {
    auto [width, height] = ctx->window.GetSize();
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

void Renderer::build_3D_render_graph() {
  GraphBuilder builder(ctx);

  auto shader = Shader::Create(ctx->graphics_context.Device(),
                               TRIANGLE_VERT_SHADER.data(),
                               TRIANGLE_FRAG_SHADER.data())
                    .value();
  builder.add_pass(
      "triangle",
      {shader,
       {RenderTarget{swapchain_extent, swapchain_image_format,
                     vk::SampleCountFlagBits::e1}}});

  render_graph = builder.compile();
}

}  // namespace wren
