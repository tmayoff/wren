#pragma once

#include <memory>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "mesh.hpp"
#include "wren/graph.hpp"
#include "wren/render_target.hpp"

namespace wren {

struct Context;

class Renderer {
 public:
  static auto Create(std::shared_ptr<Context> const &ctx)
      -> tl::expected<std::shared_ptr<Renderer>, std::error_code>;

  void draw();

  auto get_swapchain_images_views() { return swapchain_image_views; }

 private:
  explicit Renderer(std::shared_ptr<Context> const &ctx);

  auto begin_frame() -> tl::expected<uint32_t, std::error_code>;
  void end_frame(uint32_t image_index);

  auto recreate_swapchain() -> tl::expected<void, std::error_code>;

  auto choose_swapchain_format(
      std::vector<vk::SurfaceFormatKHR> const &formats)
      -> vk::SurfaceFormatKHR;
  auto choose_swapchain_presentation_mode(
      std::vector<vk::PresentModeKHR> const &formats)
      -> vk::PresentModeKHR;
  auto choose_swapchain_extent(
      vk::SurfaceCapabilitiesKHR const &surface_capabilities)
      -> vk::Extent2D;

  [[nodiscard]] auto build_3D_render_graph()
      -> tl::expected<void, std::error_code>;

  std::shared_ptr<Context> ctx;
  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> swapchain_images;
  std::vector<vk::ImageView> swapchain_image_views;

  std::shared_ptr<RenderTarget> target;

  vk::Format swapchain_image_format = vk::Format::eB8G8R8Srgb;
  vk::Extent2D swapchain_extent;

  vk::Semaphore image_available;
  vk::Semaphore render_finished;
  vk::Fence in_flight_fence;

  vk::CommandPool command_pool;

  Graph render_graph;

  Mesh m;
};

}  // namespace wren
