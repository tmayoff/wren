#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/assets/manager.hpp>

auto initialize(const std::shared_ptr<wren::Application>& app)
    -> wren::expected<void> {
  spdlog::info("Intializing shaders");

  std::vector<std::filesystem::path> asset_paths = {
#ifdef WREN_BUILD_ASSETS_DIR
      WREN_BUILD_ASSETS_DIR
#endif
  };
  wren::assets::Manager asset_manager(asset_paths);
  TRY_RESULT(const auto asset_path,
             asset_manager.find_asset("shaders/mesh.wren_shader"));

  TRY_RESULT(auto mesh_shader,
             wren::vk::Shader::create(
                 app->context()->graphics_context->Device().get(), asset_path));

  wren::GraphBuilder builder(app->context());
  builder
      .add_pass("main",
                wren::PassResources("swapchain_target")
                    .add_shader("mesh", mesh_shader)
                    .add_colour_target(),
                [](wren::RenderPass& pass, const vk::CommandBuffer& cmd) {
                  // TODO Render things here
                })
      .build()
      .value();

  return {};
}

auto main() -> int {
  spdlog::set_level(spdlog::level::debug);

  const auto& err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}", err.error().message());
    return EXIT_FAILURE;
  }

  auto app = err.value();

  const auto init_err = initialize(app);
  if (!init_err.has_value()) {
    spdlog::error("failed to initialize: {}", init_err.error().message());
    return EXIT_FAILURE;
  }

  app->run();

  return EXIT_SUCCESS;
}
