#include "editor.hpp"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <tracy/Tracy.hpp>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/renderer.hpp>
#include <wren/scene/components/mesh.hpp>
#include <wren/scene/components/tag.hpp>
#include <wren/scene/entity.hpp>
#include <wren/shaders/mesh.hpp>
#include <wren_math/geometry.hpp>

#include "filesystem_panel.hpp"
#include "inspector_panel.hpp"
#include "scene_panel.hpp"
#include "ui.hpp"
#include "wren/scene/components/transform.hpp"
#include "wren/scene/serialization.hpp"

namespace editor {

auto Editor::create(const std::shared_ptr<wren::Application> &app,
                    const std::filesystem::path &project_path)
    -> wren::expected<std::shared_ptr<Editor>> {
  const auto &ctx = app->context();

  auto editor = std::make_shared<Editor>(app->context());
  editor->editor_context_.project_path = project_path;
  editor->load_scene();

  TRY_RESULT(
      editor->mesh_shader_,
      wren::vk::Shader::create(app->context()->graphics_context->Device().get(),
                               wren::shaders::kMeshVertShader.data(),
                               wren::shaders::kMeshFragShader.data()));

  wren::Mesh m(ctx->graphics_context->Device(),
               ctx->graphics_context->allocator());
  m.shader(editor->mesh_shader_);
  auto mesh = editor->scene_->create_entity();
  mesh.add_component<wren::scene::components::MeshRenderer>(m);

  TRY_RESULT(const auto graph, editor->build_render_graph(app->context()));
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

Editor::Editor(const std::shared_ptr<wren::Context> &ctx)
    : camera_(45.F, 16.f / 9.f, 0.1, 1000.0),
      scene_(wren::scene::Scene::create()),
      wren_ctx_(ctx) {}

void Editor::on_update() {
  ZoneScoped;  // NOLINT

  if (scene_resized_.has_value()) {
    const auto &mesh_pass =
        wren_ctx_->renderer->get_graph().node_by_name("mesh")->render_pass;

    mesh_pass->resize_target(scene_resized_.value());

    const auto scene_view = wren_ctx_->renderer->get_graph()
                                .node_by_name("mesh")
                                ->render_pass->target()
                                ->image_view;

    ImGui_ImplVulkan_RemoveTexture(dset_[0]);
    dset_[0] = ImGui_ImplVulkan_AddTexture(
        texture_sampler_, scene_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    camera_.aspect(static_cast<float>(scene_resized_->x()) /
                   static_cast<float>(scene_resized_->y()));

    scene_resized_.reset();
  }

  editor::ui::begin();

  ImGui::ShowDemoWindow();

  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_MenuBar;

  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);

  ImGui::Begin("Editor", nullptr, window_flags);
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Save")) {
        // SAVE the scene

        auto file = editor_context_.project_path / "scene.wren";
        wren::scene::serialize(scene_, file);
      }

      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

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

      const auto inspector = ImGui::DockBuilderSplitNode(
          dockspace_id, ImGuiDir_Right, 0.25F, nullptr, &dockspace_id);

      ImGuiID viewer = 0;
      const auto left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left,
                                                    0.25F, nullptr, &viewer);

      ImGuiID scene = 0;
      const auto dockspace_id_bottom = ImGui::DockBuilderSplitNode(
          left, ImGuiDir_Down, 0.25, nullptr, &scene);

      ImGui::DockBuilderDockWindow("Inspector", inspector);
      ImGui::DockBuilderDockWindow("Viewer", viewer);
      ImGui::DockBuilderDockWindow("Scene", scene);
      ImGui::DockBuilderDockWindow("Filesystem", dockspace_id_bottom);

      ImGui::DockBuilderFinish(dockspace_id_bottom);
    }
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{500, 100});

  // Panels
  render_scene_panel(scene_, selected_entity_);

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

  render_inspector_panel(editor_context_, scene_, selected_entity_);

  render_filesystem_panel(editor_context_);

  ImGui::End();
  ImGui::PopStyleVar();

  editor::ui::end();
}

auto Editor::load_scene() -> wren::expected<void> {
  // Deserialize project
  std::string scene_name;

  return {};
}

auto Editor::build_render_graph(const std::shared_ptr<wren::Context> &ctx)
    -> wren::expected<wren::GraphBuilder> {
  wren::GraphBuilder builder(ctx);

  auto render_query =
      scene_->world()
          .query_builder<const wren::scene::components::Transform,
                         wren::scene::components::MeshRenderer>()
          .build();

  builder
      .add_pass(
          "mesh",
          {{
               {"mesh", mesh_shader_},
           },
           "scene_viewer"},
          [this, ctx, render_query](wren::RenderPass &pass,
                                    ::vk::CommandBuffer &cmd) {
            pass.bind_pipeline("mesh");

            struct GLOBALS {
              wren::math::Mat4f view = wren::math::Mat4f::identity();
              wren::math::Mat4f proj = wren::math::Mat4f::identity();
            };
            GLOBALS ubo{};

            // ubo.view = this->camera_.position().matrix();
            ubo.proj = this->camera_.projection();

            pass.write_scratch_buffer(cmd, 0, 0, ubo);

            render_query.each(
                [cmd, &pass, ctx](
                    const wren::scene::components::Transform &transform,
                    wren::scene::components::MeshRenderer &mesh_renderer) {
                  struct LOCALS {
                    wren::math::Mat4f model;
                  };
                  LOCALS ubo{};

                  if (!mesh_renderer.mesh.loaded()) {
                    mesh_renderer.mesh.load(ctx->graphics_context->Device(),
                                            ctx->graphics_context->allocator());
                  }

                  ubo.model = transform.matrix();

                  pass.write_scratch_buffer(cmd, 0, 1, ubo);

                  mesh_renderer.mesh.bind(cmd);
                  mesh_renderer.mesh.draw(cmd);
                });
          })
      .add_pass("ui", {{}, "swapchain_target"},
                [](wren::RenderPass &pass, ::vk::CommandBuffer &cmd) {
                  editor::ui::flush(cmd);
                });

  return builder;
}

}  // namespace editor
