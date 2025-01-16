#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>
#include <wren/assets/manager.hpp>
#include <wren/physics/ray.hpp>
#include <wren/scene/components/collider.hpp>
#include <wren/scene/components/mesh.hpp>
#include <wren/scene/entity.hpp>
#include <wren/scene/scene.hpp>

namespace components = wren::scene::components;

struct GameData {
  std::shared_ptr<wren::Application> app;
  std::shared_ptr<wren::scene::Scene> scene;
  wren::math::Mat4f ortho_proj;
};

auto initialize(const GameData& data) -> wren::expected<void> {
  spdlog::info("Intializing shaders");

  std::vector<std::filesystem::path> asset_paths = {
#ifdef WREN_BUILD_ASSETS_DIR
      WREN_BUILD_ASSETS_DIR
#endif
  };

  wren::assets::Manager asset_manager(asset_paths);
  TRY_RESULT(const auto asset_path,
             asset_manager.find_asset("shaders/mesh.wren_shader"));

  TRY_RESULT(
      auto mesh_shader,
      wren::vk::Shader::create(
          data.app->context()->graphics_context->Device().get(), asset_path));

  wren::GraphBuilder builder(data.app->context());

  // TODO Get Scene here
  auto render_query =
      data.scene->world()
          .query_builder<const wren::scene::components::Transform,
                         wren::scene::components::MeshRenderer>()
          .build();
  builder
      .add_pass(
          "main",
          wren::PassResources("swapchain_target")
              .add_shader("mesh", mesh_shader)
              .add_colour_target(),
          [data, app = data.app, mesh_shader, render_query](
              wren::RenderPass& pass, const vk::CommandBuffer& cmd) {
            struct GLOBALS {
              wren::math::Mat4f view = wren::math::Mat4f::identity();
              wren::math::Mat4f proj = wren::math::Mat4f::identity();
            };
            GLOBALS ubo{};

            // ubo.view = this->camera_.transform().matrix();
            ubo.proj = data.ortho_proj;

            pass.bind_pipeline("mesh");
            pass.write_scratch_buffer(cmd, 0, 0, ubo);

            render_query.each(
                [cmd, app, mesh_shader](
                    const wren::scene::components::Transform& transform,
                    wren::scene::components::MeshRenderer& mesh_renderer) {
                  mesh_renderer.bind(app->context(), mesh_shader, cmd,
                                     transform.matrix());
                });
          })
      .build()
      .value();

  return {};
}

void update(const std::shared_ptr<wren::scene::Scene>& scene);

auto main() -> int {
  spdlog::set_level(spdlog::level::debug);

  const auto& err = wren::Application::Create("nand");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}", err.error().message());
    return EXIT_FAILURE;
  }

  const auto app = err.value();
  const auto scene = wren::scene::Scene::create();

  GameData data{
      .app = app,
      .scene = scene,
      .ortho_proj = wren::math::ortho(0.0F, 80.0F, 0.0F, 60.0F, 0.1F, 100.0F)};

  const auto init_err = initialize(data);
  if (!init_err.has_value()) {
    spdlog::error("failed to initialize: {}", init_err.error().message());
    return EXIT_FAILURE;
  }

  auto quad = scene->create_entity("Quad");
  quad.add_component<components::MeshRenderer>();
  auto& mesh_renderer = quad.get_component<components::MeshRenderer>();
  mesh_renderer.update_mesh(wren::Mesh::create_quad());

  quad.add_component<wren::scene::components::BoxCollider2D::Ptr>(
      new components::BoxCollider2D());
  auto& collider = quad.get_component<components::BoxCollider2D>();

  app->add_callback_to_phase(wren::CallbackPhase::Update,
                             [scene]() { update(scene); });

  quad.get_component<components::Transform>().position.z(1);

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
