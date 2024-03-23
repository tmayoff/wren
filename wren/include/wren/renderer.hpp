#pragma once

#include <memory>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "mesh.hpp"
#include "wren/graph.hpp"
#include "wren/pipeline.hpp"
#include "wren/render_target.hpp"

namespace wren {

struct Context;

class Renderer {
  static constexpr std::string_view SWAPCHAIN_RENDERTARGET_NAME =
      "swapchain_target";

 public:
  static auto New(std::shared_ptr<Context> const &ctx)
      -> expected<std::shared_ptr<Renderer>>;

  void draw();

  auto set_graph_builder(GraphBuilder const &builder) {
    render_graph = builder.compile().value();
  }

  auto swapchain_images_views() { return swapchain_image_views_; }

  auto render_targets() { return render_targets_; }

 private:
  explicit Renderer(std::shared_ptr<Context> const &ctx);

  auto begin_frame() -> expected<uint32_t>;
  void end_frame(uint32_t image_index);

  auto recreate_swapchain() -> expected<void>;

  auto choose_swapchain_format(
      std::vector<VK_NS::SurfaceFormatKHR> const &formats)
      -> VK_NS::SurfaceFormatKHR;
  auto choose_swapchain_presentation_mode(
      std::vector<VK_NS::PresentModeKHR> const &formats)
      -> VK_NS::PresentModeKHR;
  auto choose_swapchain_extent(
      VK_NS::SurfaceCapabilitiesKHR const &surface_capabilities)
      -> VK_NS::Extent2D;

  std::unordered_map<std::string, std::shared_ptr<Pipeline>>
      pipelines;
  std::shared_ptr<Context> ctx;
  VK_NS::SwapchainKHR swapchain;
  std::vector<VK_NS::Image> swapchain_images;
  std::vector<VK_NS::ImageView> swapchain_image_views_;

  std::unordered_map<std::string, std::shared_ptr<RenderTarget>>
      render_targets_;

  VK_NS::Format swapchain_image_format = VK_NS::Format::eB8G8R8Srgb;
  VK_NS::Extent2D swapchain_extent;

  VK_NS::Semaphore image_available;
  VK_NS::Semaphore render_finished;
  VK_NS::Fence in_flight_fence;

  VK_NS::CommandPool command_pool;

  Graph render_graph;

  Mesh m;
};

}  // namespace wren
