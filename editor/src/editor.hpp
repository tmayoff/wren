#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <wren/mesh.hpp>
#include <wren_math/vector.hpp>
#include <wren_utils/errors.hpp>

#include "camera.hpp"

namespace wren {
struct Context;
class Application;
class GraphBuilder;
class RenderTarget;
}  // namespace wren

namespace editor {

class Editor {
 public:
  static auto create(const std::shared_ptr<wren::Application> &app)
      -> wren::expected<std::shared_ptr<Editor>>;

  Editor(const std::shared_ptr<wren::Context> &ctx) : camera_(45.F, 16.f/9.f, 0.1, 1000.0f), ctx_(ctx) {}

  auto build_ui_render_graph(const std::shared_ptr<wren::Context> &ctx)
      -> wren::expected<wren::GraphBuilder>;

  void on_update();

 private:
  Camera camera_;

  std::shared_ptr<wren::Context> ctx_;
  wren::Mesh mesh_;

  std::vector<VkDescriptorSet> dset_{};
  vk::Sampler texture_sampler_;

  std::optional<wren::math::vec2i> scene_resized_;
  wren::math::vec2i last_scene_size_;
};

}  // namespace editor
