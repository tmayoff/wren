#include "editor.hpp"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <tracy/Tracy.hpp>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/renderer.hpp>
#include <wren/shaders/mesh.hpp>
#include <wren_math/geometry.hpp>

#include "ui.hpp"

namespace editor {

auto Editor::create(const std::shared_ptr<wren::Application> &app)
    -> wren::expected<std::shared_ptr<Editor>> {
  auto editor = std::make_shared<Editor>(app->context());

  TRY_RESULT(const auto graph, editor->build_ui_render_graph(app->context()));
  app->context()->renderer->set_graph_builder(graph);

  editor::ui::init(app->context());

  vk::SamplerCreateInfo sampler_info{};

  auto t = app->context()->graphics_context->Device().get().createSampler(
      sampler_info);
  editor->texture_sampler_ = t.value;

  editor->dset_.resize(1);
  const auto scene_view = app->context()
                              ->renderer->get_graph()
                              .node_by_name("mesh")
                              ->render_pass->target()
                              ->image_view;
  editor->dset_[0] =
      ImGui_ImplVulkan_AddTexture(editor->texture_sampler_, scene_view,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  app->add_callback_to_phase(wren::CallbackPhase::Update,
                             [editor]() { editor->on_update(); });

  return editor;
}

void Editor::on_update() {
  ZoneScoped;  // NOLINT

  if (scene_resized_.has_value()) {
    const auto &mesh_pass =
        ctx_->renderer->get_graph().node_by_name("mesh")->render_pass;

    mesh_pass->resize_target(scene_resized_.value());

    const auto scene_view = ctx_->renderer->get_graph()
                                .node_by_name("mesh")
                                ->render_pass->target()
                                ->image_view;

    ImGui_ImplVulkan_RemoveTexture(dset_[0]);
    dset_[0] = ImGui_ImplVulkan_AddTexture(
        texture_sampler_, scene_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    camera_.aspect(
        static_cast<float>(scene_resized_->x() / scene_resized_->y()));

    scene_resized_.reset();
  }

  editor::ui::begin();

  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);

  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoBringToFrontOnFocus |
                  ImGuiWindowFlags_NoNavFocus;

  ImGui::Begin("Editor", nullptr, window_flags);
  ImGui::PopStyleVar(2);

  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("Editor");
    ImGui::DockSpace(dockspace_id, ImVec2{0.0F, 0.0F}, dockspace_flags);

    static bool first_time = true;
    if (first_time) {
      first_time = false;

      ImGui::DockBuilderRemoveNode(dockspace_id);
      ImGui::DockBuilderAddNode(dockspace_id,
                                dockspace_flags | ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

      const auto dockspace_id_bottom = ImGui::DockBuilderSplitNode(
          dockspace_id, ImGuiDir_Down, 0.25, nullptr, &dockspace_id);

      const auto dockspace_id_right = ImGui::DockBuilderSplitNode(
          dockspace_id, ImGuiDir_Right, 0.25F, nullptr, &dockspace_id);
      const auto dockspace_id_left = ImGui::DockBuilderSplitNode(
          dockspace_id, ImGuiDir_Left, 0.25F, nullptr, &dockspace_id);

      ImGui::DockBuilderDockWindow("Inspector", dockspace_id_right);
      ImGui::DockBuilderDockWindow("Viewer", dockspace_id);
      ImGui::DockBuilderDockWindow("Scene", dockspace_id_left);
      ImGui::DockBuilderDockWindow("Filesystem", dockspace_id_bottom);

      ImGui::DockBuilderFinish(dockspace_id_bottom);
    }
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{500, 100});
  ImGui::Begin("Scene");
  ImGui::End();

  ImGui::Begin("Viewer");
  auto curr_size = ImGui::GetContentRegionAvail();
  auto curr_size_vec = wren::math::vec2i{static_cast<int>(curr_size.x),
                                         static_cast<int>(curr_size.y)};
  if (curr_size_vec != last_scene_size_) {
    scene_resized_ = curr_size_vec;
    last_scene_size_ = curr_size_vec;
  }

  ImGui::Image(dset_[0], {static_cast<float>(last_scene_size_.x()),
                          static_cast<float>(last_scene_size_.y())});
  ImGui::End();

  ImGui::Begin("Inspector");
  ImGui::End();

  ImGui::Begin("Filesystem");
  ImGui::End();

  ImGui::End();
  ImGui::PopStyleVar();

  editor::ui::end();
}

auto Editor::build_ui_render_graph(const std::shared_ptr<wren::Context> &ctx)
    -> wren::expected<wren::GraphBuilder> {
  wren::GraphBuilder builder(ctx);

  TRY_RESULT(const auto mesh_shader,
             wren::vk::Shader::create(ctx->graphics_context->Device().get(),
                                      wren::shaders::kMeshVertShader.data(),
                                      wren::shaders::kMeshFragShader.data()));

  mesh_ = wren::Mesh(ctx->graphics_context->Device(),
                     ctx->graphics_context->allocator());
  mesh_.shader(mesh_shader);

  builder
      .add_pass("mesh",
                {{
                     {"mesh", mesh_shader},
                 },
                 "scene_viewer"},
                [this, ctx](wren::RenderPass &pass, ::vk::CommandBuffer &cmd) {
                  pass.bind_pipeline("mesh");

                  // TODO use the camera
                  struct GLOBALS {
                    wren::math::mat4f view;
                    wren::math::mat4f proj;
                  };
                  GLOBALS ubo{};

                  ubo.view =
                      wren::math::look_at(wren::math::vec3f(2.0f, 2.0f, 2.0f),
                                          wren::math::vec3f(0.0f, 0.0f, 0.0f),
                                          wren::math::vec3f::UnitZ());

                  ubo.proj = this->camera_.projection();

                  pass.write_scratch_buffer(cmd, 0, 0, ubo);

                  mesh_.bind(cmd);
                  mesh_.draw(cmd);
                })
      .add_pass("ui", {{}, "swapchain_target"},
                [](wren::RenderPass &pass, ::vk::CommandBuffer &cmd) {
                  editor::ui::flush(cmd);
                });

  return builder;
}

}  // namespace editor
