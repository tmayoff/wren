#include <spdlog/fmt/std.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/event.hpp>
#include <wren/keycode.hpp>
#include <wren/render_pass.hpp>
#include <wren/shaders/mesh.hpp>
#include <wren/shaders/triangle.hpp>
#include <wren_gui/instance.hpp>
#include <wren_gui/shader.hpp>
#include <wren_utils/errors.hpp>
#include <wrenm/utils.hpp>

class Scene {
 public:
  Scene(std::shared_ptr<wren::Context> const &ctx)
      : ctx(ctx),
        mesh(ctx->graphics_context->Device(),
             ctx->graphics_context->allocator()) {
    auto ui_shader_res = wren::vk::Shader::Create(
        ctx->graphics_context->Device().get(),
        wren::gui::VERTEX_SHADER.data(),
        wren::gui::FRAGMENT_SHADER.data());
    if (!ui_shader_res.has_value()) {
      spdlog::error("failed to create UI shader {}",
                    ui_shader_res.error());
      throw std::runtime_error(
          fmt::format("{}", ui_shader_res.error()));
    }

    ctx->event_dispatcher.on<wren::Event::MouseMoved>(
        [this](auto &e) {
          if (e.relative)
            gui_instance->IO().mouse_position_rel = {e.x, e.y};
          else
            gui_instance->IO().mouse_position = {e.x, e.y};
        });

    ctx->event_dispatcher.on<wren::Event::MouseButtonUp>(
        [this](auto const &e) {
          if (e.button == wren::MouseCode::Left)
            gui_instance->IO().left_mouse = false;
        });

    ctx->event_dispatcher.on<wren::Event::MouseButtonDown>(
        [this](auto const &e) {
          if (e.button == wren::MouseCode::Left)
            gui_instance->IO().left_mouse = true;
        });

    gui_instance = std::make_shared<wren::gui::Instance>(
        ui_shader_res.value(), ctx->graphics_context->Device().get(),
        ctx->graphics_context->allocator(),
        ctx->graphics_context->Device().command_pool(),
        ctx->graphics_context->Device().get_graphics_queue());
  }

  auto build_3D_render_graph(
      std::shared_ptr<wren::Context> const &ctx)
      -> wren::expected<wren::GraphBuilder>;

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
  // spdlog::info("Position: ({} {}), Moved ({}, {}), Mouse down: {}",
  //              gui_instance->IO().mouse_position.x(),
  //              gui_instance->IO().mouse_position.y(),
  //              gui_instance->IO().mouse_position_rel.x(),
  //              gui_instance->IO().mouse_position_rel.y(),
  //              gui_instance->IO().left_mouse);

  gui_instance->Begin();

  gui_instance->BeginWindow("Main", {400, 400});

  // TODO Render text

  gui_instance->EndWindow();

  gui_instance->End();
}

auto Scene::build_3D_render_graph(
    std::shared_ptr<wren::Context> const &ctx)
    -> wren::expected<wren::GraphBuilder> {
  wren::GraphBuilder builder(ctx);

  ERR_PROP(auto mesh_shader,
           wren::vk::Shader::Create(
               ctx->graphics_context->Device().get(),
               wren::shaders::MESH_VERT_SHADER.data(),
               wren::shaders::MESH_FRAG_SHADER.data()));

  mesh.shader(mesh_shader);
  builder.add_pass(
      "scene",
      {{{"mesh", mesh_shader}, {"ui", gui_instance->shader()}},
       "swapchain_target"},
      [this](wren::RenderPass &pass, VK_NS::CommandBuffer &cmd) {
        pass.bind_pipeline("mesh");
        mesh.bind(cmd);
        mesh.draw(cmd);

        pass.bind_pipeline("ui");
        gui_instance->resize_viewport(pass.current_target_size());
        gui_instance->flush(cmd);
      });

  return builder;
}
