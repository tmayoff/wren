#pragma once

#include <memory>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "wren/graph.hpp"

namespace wren {

struct Context;

class Renderer {
 public:
  static auto Create(const std::shared_ptr<Context> &ctx)
      -> tl::expected<std::shared_ptr<Renderer>, std::error_code>;

  void draw();

  auto get_swapchain_images_views() { return swapchain_image_views; }

  void on_window_resize(const std::pair<float, float> &size);

 private:
  explicit Renderer(const std::shared_ptr<Context> &ctx) : ctx(ctx) {}

  void begin_frame();
  void end_frame();

  auto recreate_swapchain() -> tl::expected<void, std::error_code>;

  auto choose_swapchain_format(
      const std::vector<vk::SurfaceFormatKHR> &formats)
      -> vk::SurfaceFormatKHR;
  auto choose_swapchain_presentation_mode(
      const std::vector<vk::PresentModeKHR> &formats)
      -> vk::PresentModeKHR;
  auto choose_swapchain_extent(
      const vk::SurfaceCapabilitiesKHR &surface_capabilities)
      -> vk::Extent2D;

  void build_3D_render_graph();

  std::shared_ptr<Context> ctx;
  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> swapchain_images;
  std::vector<vk::ImageView> swapchain_image_views;

  std::vector<std::shared_ptr<RenderTarget>> targets;

  vk::Format swapchain_image_format = vk::Format::eB8G8R8Srgb;
  vk::Extent2D swapchain_extent;

  vk::Semaphore image_available;
  vk::Semaphore render_finished;
  vk::Fence in_flight_fence;

  Graph render_graph;
};

}  // namespace wren
