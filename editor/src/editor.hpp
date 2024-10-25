#pragma once

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <filesystem>
#include <memory>
#include <tracy/Tracy.hpp>
#include <vulkan/vulkan.hpp>
#include <wren/application.hpp>
#include <wren/math/vector.hpp>
#include <wren/mesh.hpp>
#include <wren/scene/components/mesh.hpp>
#include <wren/scene/deserialization.hpp>
#include <wren/scene/scene.hpp>
#include <wren/scene/serialization.hpp>
#include <wren/utils/result.hpp>

#include "camera.hpp"
#include "context.hpp"
#include "ui.hpp"

namespace editor {

class Editor {
 public:
  static auto create(const std::shared_ptr<wren::Application> &app,
                     const std::filesystem::path &project_path)
      -> wren::expected<std::shared_ptr<Editor>>;

  Editor(const std::shared_ptr<wren::Context> &ctx);

  void on_update();

 private:
  auto load_scene() -> wren::expected<void>;

  auto build_render_graph(const std::shared_ptr<wren::Context> &ctx)
      -> wren::expected<wren::GraphBuilder>;

  // Project
  Context editor_context_;

  // Editor camera
  Camera camera_;

  // Panels
  std::optional<flecs::entity> selected_entity_;

  // Scene management
  std::shared_ptr<wren::scene::Scene> scene_;

  std::shared_ptr<wren::Context> wren_ctx_;

  std::shared_ptr<wren::vk::Shader> mesh_shader_;
  std::shared_ptr<wren::vk::Shader> viewer_shader_;

  // Scene viewer
  std::vector<VkDescriptorSet> dset_{};
  vk::Sampler texture_sampler_;

  std::optional<wren::math::vec2i> scene_resized_;
  wren::math::vec2i last_scene_size_;
};

}  // namespace editor
