#include <spdlog/fmt/std.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/shaders/mesh.hpp>
#include <wren/shaders/triangle.hpp>
#include <wren_gui/instance.hpp>
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

  ERR_PROP(
      auto mesh_shader,
      wren::Shader::Create(ctx->graphics_context->Device(),
                           wren::shaders::MESH_VERT_SHADER.data(),
                           wren::shaders::MESH_FRAG_SHADER.data()));

  ERR_PROP(auto ui_shader,
           wren::Shader::Create(
               ctx->graphics_context->Device(),
               wren::shaders::TRIANGLE_VERT_SHADER.data(),
               wren::shaders::TRIANGLE_FRAG_SHADER.data()));

  mesh.shader(mesh_shader);
  builder.add_pass("scene", {mesh_shader, "swapchain_target"},
                   [this](VK_NS::CommandBuffer &cmd) {
                     mesh.bind(cmd);
                     mesh.draw(cmd);

                     gui_instance->draw(cmd);
                   });

  return builder;
}
