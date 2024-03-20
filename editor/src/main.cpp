#include <spdlog/fmt/std.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/shaders/mesh.hpp>

#include "wren/utils/errors.hpp"

auto build_3D_render_graph(std::shared_ptr<wren::Context> const &ctx)
    -> tl::expected<wren::GraphBuilder, std::error_code>;

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

  auto g_err = build_3D_render_graph(app->context());
  if (!g_err.has_value()) {
    spdlog::error("failed to create application: {}",
                  err.error().message());
    return EXIT_FAILURE;
  }

  app->run();

  return EXIT_SUCCESS;
}

auto build_3D_render_graph(std::shared_ptr<wren::Context> const &ctx)
    -> tl::expected<wren::GraphBuilder, std::error_code> {
  wren::GraphBuilder builder(ctx);

  ERR_PROP(
      auto mesh_shader,
      wren::Shader::Create(ctx->graphics_context->Device(),
                           wren::shaders::MESH_VERT_SHADER.data(),
                           wren::shaders::MESH_FRAG_SHADER.data()));

  ERR_PROP(
      auto ui_shader,
      wren::Shader::Create(ctx->graphics_context->Device(),
                           wren::shaders::MESH_VERT_SHADER.data(),
                           wren::shaders::MESH_FRAG_SHADER.data()));

  builder.add_pass("3d_mesh", {mesh_shader},
                   [](vk::CommandBuffer &cmd) {});

  builder.add_pass("ui", {ui_shader}, [](vk::CommandBuffer &cmd) {});

  ERR_PROP(auto render_graph, builder.compile());

  return builder;
}
