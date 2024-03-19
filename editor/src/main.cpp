#include <spdlog/fmt/std.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/shaders/mesh.hpp>

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

  app->run();

  return EXIT_SUCCESS;
}

auto build_3D_render_graph(std::shared_ptr<wren::Context> const &ctx)
    -> tl::expected<void, std::error_code> {
  wren::GraphBuilder builder(ctx);

  auto shader =
      wren::Shader::Create(ctx->graphics_context->Device(),
                           wren::shaders::MESH_VERT_SHADER.data(),
                           wren::shaders::MESH_FRAG_SHADER.data())

          .value();

  builder.add_pass("triangle", {shader},
                   [](vk::CommandBuffer &cmd) {});

  ERR_PROP(auto render_graph, builder.compile());

  return {};
}
