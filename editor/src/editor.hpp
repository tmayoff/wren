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
  static auto create(const std::shared_ptr<wren::Application> &app)
      -> wren::expected<std::shared_ptr<Editor>>;

  Editor(const std::shared_ptr<wren::Context> &ctx) : ctx_(ctx) {}

  auto build_ui_render_graph(const std::shared_ptr<wren::Context> &ctx)
      -> wren::expected<wren::GraphBuilder>;

  void on_update();

 private:
  auto resize_target(const std::shared_ptr<wren::RenderTarget> &target)
      -> wren::expected<void>;

  std::shared_ptr<wren::Context> ctx_;
  wren::Mesh mesh_;

  std::vector<VkDescriptorSet> dset_{};
  vk::Sampler texture_sampler_;

  std::optional<wren::math::vec2f> scene_resized_;
  wren::math::vec2f last_scene_size_;
};
