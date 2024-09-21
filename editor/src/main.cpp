#include <imgui.h>
#include <imgui_internal.h>

#include <backward.hpp>
#include <memory>
#include <tracy/Tracy.hpp>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/graph.hpp>
#include <wren/shaders/mesh.hpp>

#include "ui.hpp"

backward::SignalHandling const sh;

class Scene {
 public:
  Scene(std::shared_ptr<wren::Context> const &ctx) : ctx(ctx) {}

  auto build_ui_render_graph(
      std::shared_ptr<wren::Context> const &ctx)
      -> wren::expected<wren::GraphBuilder>;

  // auto build_3D_render_graph(
  //     std::shared_ptr<wren::Context> const &ctx)
  //     -> wren::expected<wren::GraphBuilder>;

  void on_update();

 private:
  std::shared_ptr<wren::Context> ctx;
  wren::Mesh mesh;

  // Image image
};

auto main() -> int {
  auto const &err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}",
                  err.error().message());
    return EXIT_FAILURE;
  }

  auto app = err.value();

  // TODO Build 3D rendergraph, be able to check resources if non
  // match the output framebuffer add a pass specifically for
  // transitioning into the presentable layout
  Scene s(app->context());
  auto g_err = s.build_ui_render_graph(app->context());
  if (!g_err.has_value()) {
    spdlog::error("failed to create application: {}",
                  err.error().message());
    return EXIT_FAILURE;
  }

  app->context()->renderer->set_graph_builder(g_err.value());

  editor::ui::init(app->context());

  app->add_callback_to_phase(wren::CallbackPhase::Update,
                             [&s]() { s.on_update(); });

  app->run();

  return EXIT_SUCCESS;
}

void Scene::on_update() {
  ZoneScoped;  // NOLINT
  editor::ui::begin();

  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

  ImGuiViewport const *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);

  window_flags |=
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;

  ImGui::Begin("Editor", nullptr, window_flags);
  ImGui::PopStyleVar(2);

  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("Editor");
    ImGui::DockSpace(dockspace_id, ImVec2{0.0F, 0.0F},
                     dockspace_flags);

    static bool first_time = true;
    if (first_time) {
      first_time = false;

      ImGui::DockBuilderRemoveNode(dockspace_id);
      ImGui::DockBuilderAddNode(
          dockspace_id,
          dockspace_flags | ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

      auto const dockspace_id_bottom = ImGui::DockBuilderSplitNode(
          dockspace_id, ImGuiDir_Down, 0.25, nullptr, &dockspace_id);

      auto const dockspace_id_right =
          ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right,
                                      0.25F, nullptr, &dockspace_id);
      auto const dockspace_id_left = ImGui::DockBuilderSplitNode(
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
  ImGui::Text("%s", "Scene go here");
  ImGui::End();

  ImGui::Begin("Inspector");
  ImGui::End();

  ImGui::Begin("Filesystem");
  ImGui::End();

  ImGui::End();
  ImGui::PopStyleVar();

  editor::ui::end();
}

auto Scene::build_ui_render_graph(
    std::shared_ptr<wren::Context> const &ctx)
    -> wren::expected<wren::GraphBuilder> {
  wren::GraphBuilder builder(ctx);

  TRY_RESULT(auto mesh_shader,
             wren::vk::Shader::Create(
                 ctx->graphics_context->Device().get(),
                 wren::shaders::MESH_VERT_SHADER.data(),
                 wren::shaders::MESH_FRAG_SHADER.data()));

  auto target = std::make_shared<wren::RenderTarget>(
      VK_NS::Extent2D{245, 256}, VK_NS::Format::eB8G8R8A8Srgb,
      VK_NS::SampleCountFlagBits::e1, nullptr);

  VK_NS::ImageCreateInfo image_info(
      {}, VK_NS::ImageType::e2D, target->format,
      VK_NS::Extent3D(target->size.width, target->size.height, 1));

  VK_TRY_RESULT(
      image,
      ctx->graphics_context->Device().get().createImage(image_info));

  VK_NS::ImageViewCreateInfo image_view_info(
      {}, image, VK_NS::ImageViewType::e2D);

  VK_TRY_RESULT(image_view,
                ctx->graphics_context->Device().get().createImageView(
                    image_view_info));

  mesh.shader(mesh_shader);

  builder
      .add_pass(
          "ui", {{}, "swapchain_target"},
          [](wren::RenderPass &pass, VK_NS::CommandBuffer &cmd) {
            editor::ui::flush(cmd);
          })
      .add_pass(
          "mesh",
          {
              {
                  {"mesh", mesh_shader},
              },
              "scene_viewer",
              target,
          },
          [this](wren::RenderPass &pass, VK_NS::CommandBuffer &cmd) {
            // pass.bind_pipeline("mesh");
            // mesh.bind(cmd);
            // mesh.draw(cmd);
          });

  return builder;
}
