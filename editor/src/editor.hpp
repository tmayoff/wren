#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <wren/graph.hpp>
#include <wren/mesh.hpp>
#include <wren_utils/errors.hpp>

namespace wren {
struct Context;
class Application;
}  // namespace wren

class Editor {
 public:
  std::vector<VkDescriptorSet> dset{};
  vk::Sampler texture_sampler;
  ::vk::ImageView scene_view;

  static auto New(std::shared_ptr<wren::Application> const &app)
      -> wren::expected<std::shared_ptr<Editor>>;

  Editor(std::shared_ptr<wren::Context> const &ctx) : ctx(ctx) {}

  auto build_ui_render_graph(
      std::shared_ptr<wren::Context> const &ctx)
      -> wren::expected<wren::GraphBuilder>;

  void on_update();

 private:
  std::shared_ptr<wren::Context> ctx;
  wren::Mesh mesh;

  VkImage scene_image{};

  VmaAllocation scene_alloc_{};

  wren::RenderTarget scene_target;
};
