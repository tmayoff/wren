#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <wren/mesh.hpp>
#include <wren/scene/scene.hpp>
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

  Editor(const std::shared_ptr<wren::Context> &ctx);

  auto build_ui_render_graph(const std::shared_ptr<wren::Context> &ctx)
      -> wren::expected<wren::GraphBuilder>;

  void on_update();

 private:
  // Editor camera
  Camera camera_;

  // Panels
  std::optional<entt::entity> selected_entity_;

  // Scene management
  std::shared_ptr<wren::scene::Scene> scene_;

  std::shared_ptr<wren::Context> ctx_;

  std::shared_ptr<wren::vk::Shader> mesh_shader_;

  // Scene viewer
  std::vector<VkDescriptorSet> dset_{};
  vk::Sampler texture_sampler_;

  std::optional<wren::math::vec2i> scene_resized_;
  wren::math::vec2i last_scene_size_;
};

}  // namespace editor
