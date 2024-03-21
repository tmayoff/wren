#include <spdlog/fmt/std.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/shaders/mesh.hpp>
#include <wren/shaders/triangle.hpp>
#include <wren/utils/errors.hpp>

class Scene {
 public:
  Scene(std::shared_ptr<wren::Context> const &ctx)
      : ctx(ctx),
        mesh(ctx->graphics_context->Device(),
             ctx->graphics_context->allocator()) {}

  auto build_3D_render_graph(
      std::shared_ptr<wren::Context> const &ctx)
      -> tl::expected<wren::GraphBuilder, std::error_code>;

 private:
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

  app->run();

  return EXIT_SUCCESS;
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

  // mesh.shader(mesh_shader);
  // builder.add_pass("3d_mesh", {mesh_shader, "mesh_output"},
  //                  [this](vk::CommandBuffer &cmd) {
  //                    mesh.bind(cmd);
  //                    mesh.draw(cmd);
  //                  });

  builder.add_pass(
      "ui", {ui_shader, "swapchain_target"},
      [](vk::CommandBuffer &cmd) { cmd.draw(3, 1, 0, 0); });

  return builder;
}
