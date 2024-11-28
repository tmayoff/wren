#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/assets/manager.hpp>
#include <wren/physics/ray.hpp>
#include <wren/scene/components/collider.hpp>
#include <wren/scene/entity.hpp>
#include <wren/scene/scene.hpp>

namespace components = wren::scene::components;

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

void update(const std::shared_ptr<wren::scene::Scene>& scene);

auto main() -> int {
  spdlog::set_level(spdlog::level::debug);

  const auto& err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}", err.error().message());
    return EXIT_FAILURE;
  }

  const auto app = err.value();

  const auto init_err = initialize(app);
  if (!init_err.has_value()) {
    spdlog::error("failed to initialize: {}", init_err.error().message());
    return EXIT_FAILURE;
  }

  const auto scene = wren::scene::Scene::create();

  auto quad = scene->create_entity("Quad");
  quad.add_component<wren::scene::components::BoxCollider2D::Ptr>(
      new components::BoxCollider2D());
  auto& collider = quad.get_component<components::BoxCollider2D>();

  app->add_callback_to_phase(wren::CallbackPhase::Update,
                             [scene]() { update(scene); });

  app->run();

  return EXIT_SUCCESS;
}

void update(const std::shared_ptr<wren::scene::Scene>& scene) {
  wren::physics::RayHit hit;
  wren::physics::Ray ray;
  ray.origin = wren::math::Vec3f{100, 0, 1};
  ray.direction = wren::math::Vec3f{0, 0, -1};
  if (wren::physics::raycast(scene->world(), ray, hit)) {
    spdlog::info("Hit object");
  }
}
