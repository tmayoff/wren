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
  auto get_graph() const { return render_graph; }

  auto swapchain_images_views() const {
    return swapchain_image_views_;
  }

  auto render_targets() const { return render_targets_; }
  auto add_target(std::string const &name,
                  std::shared_ptr<RenderTarget> const &target) {
    render_targets_.emplace(name, target);
  }

  auto submit_command_buffer(
      std::function<void(::vk::CommandBuffer &)> const &cmd_buf)
      -> expected<void>;

 private:
  explicit Renderer(std::shared_ptr<Context> const &ctx);

  auto begin_frame() -> expected<uint32_t>;
  void end_frame(uint32_t image_index);

  auto recreate_swapchain() -> expected<void>;

  auto choose_swapchain_format(
      std::vector<::vk::SurfaceFormatKHR> const &formats)
      -> ::vk::SurfaceFormatKHR;
  auto choose_swapchain_presentation_mode(
      std::vector<::vk::PresentModeKHR> const &formats)
      -> ::vk::PresentModeKHR;
  auto choose_swapchain_extent(
      ::vk::SurfaceCapabilitiesKHR const &surface_capabilities)
      -> ::vk::Extent2D;

  std::unordered_map<std::string, std::shared_ptr<Pipeline>>
      pipelines;
  std::shared_ptr<Context> ctx_;
  ::vk::SwapchainKHR swapchain;
  std::vector<::vk::Image> swapchain_images;
  std::vector<::vk::ImageView> swapchain_image_views_;

  std::unordered_map<std::string, std::shared_ptr<RenderTarget>>
      render_targets_;

  ::vk::Format swapchain_image_format = ::vk::Format::eB8G8R8Srgb;
  ::vk::Extent2D swapchain_extent;

  ::vk::Semaphore image_available;
  ::vk::Semaphore render_finished;
  ::vk::Fence in_flight_fence;

  ::vk::CommandPool command_pool_;
  ::vk::CommandBuffer one_time_cmd_buffer;

  Graph render_graph;

  Mesh m;
};

}  // namespace wren
