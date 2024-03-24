#include <spdlog/fmt/std.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/render_pass.hpp>
#include <wren/shaders/mesh.hpp>
#include <wren/shaders/triangle.hpp>
#include <wren_gui/instance.hpp>
#include <wren_gui/shader.hpp>
#include <wren_utils/errors.hpp>

class Scene {
 public:
  Scene(std::shared_ptr<wren::Context> const &ctx)
      : ctx(ctx),
        mesh(ctx->graphics_context->Device(),
             ctx->graphics_context->allocator()),
        gui_instance(std::make_shared<wren::gui::Instance>(
            ctx->graphics_context->Device().get(),
            ctx->graphics_context->allocator(),
            ctx->graphics_context->Device().command_pool(),
            ctx->graphics_context->Device().get_graphics_queue())) {}

  auto build_3D_render_graph(
      std::shared_ptr<wren::Context> const &ctx)
      -> tl::expected<wren::GraphBuilder, std::error_code>;

  void on_update();

 private:
  std::shared_ptr<wren::gui::Instance> gui_instance;
  std::shared_ptr<wren::Context> ctx;
  wren::Mesh mesh;
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
  auto g_err = s.build_3D_render_graph(app->context());
  if (!g_err.has_value()) {
    spdlog::error("failed to create application: {}",
                  err.error().message());
    return EXIT_FAILURE;
  }

  app->context()->renderer->set_graph_builder(g_err.value());

  app->add_callback_to_phase(wren::CallbackPhase::Update,
                             [&s]() { s.on_update(); });

  app->run();

  return EXIT_SUCCESS;
}

void Scene::on_update() {
  gui_instance->BeginWindow();

  // TODO Render text

  gui_instance->EndWindow();
}

auto Scene::build_3D_render_graph(
    std::shared_ptr<wren::Context> const &ctx)
    -> tl::expected<wren::GraphBuilder, std::error_code> {
  wren::GraphBuilder builder(ctx);

  ERR_PROP(auto mesh_shader,
           wren::vk::Shader::Create(
               ctx->graphics_context->Device().get(),
               wren::shaders::MESH_VERT_SHADER.data(),
               wren::shaders::MESH_FRAG_SHADER.data()));

  ERR_PROP(auto ui_shader, wren::vk::Shader::Create(
                               ctx->graphics_context->Device().get(),
                               wren::gui::VERTEX_SHADER.data(),
                               wren::gui::FRAGMENT_SHADER.data()));

  mesh.shader(mesh_shader);
  builder.add_pass(
      "scene",
      {{{"mesh", mesh_shader}, {"ui", ui_shader}},
       "swapchain_target"},
      [this](wren::RenderPass &pass, VK_NS::CommandBuffer &cmd) {
        pass.bind_pipeline("mesh");
        mesh.bind(cmd);
        mesh.draw(cmd);

        pass.bind_pipeline("ui");
        gui_instance->draw(cmd);
      });

  return builder;
}
