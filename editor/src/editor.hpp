#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <wren/mesh.hpp>
#include <wren_math/vector.hpp>
#include <wren_utils/errors.hpp>

namespace wren {
struct Context;
class Application;
class GraphBuilder;
class RenderTarget;
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
  std::shared_ptr<wren::Context> ctx_;
  wren::Mesh mesh_;

  std::vector<VkDescriptorSet> dset_{};
  vk::Sampler texture_sampler_;

  std::optional<wren::math::vec2i> scene_resized_;
  wren::math::vec2i last_scene_size_;
};
