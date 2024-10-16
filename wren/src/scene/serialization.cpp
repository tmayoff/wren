#include "scene/serialization.hpp"

#include <spdlog/spdlog.h>

#include <toml++/toml.hpp>

#include "scene/components/mesh.hpp"
#include "scene/components/transform.hpp"
#include "scene/entity.hpp"

namespace wren::scene {

void serialize_id(toml::table& out, const flecs::entity& entity,
                  const flecs::id& id);
void serialize(toml::table& table, const math::Vec3f& vec);
void serialize(toml::table& table, const components::Transform& transform);
void serialize(toml::table& table,
               const components::MeshRenderer& mesh_renderer);

auto serialize(const std::shared_ptr<Scene>& scene,
               const std::filesystem::path& out_file) -> expected<void> {
  auto scene_tbl = toml::table{
      {"version", "0.1"},
  };
  toml::array entities{};

  std::ofstream out(out_file);

  auto q = scene->world().query_builder<components::Transform>().build();
  q.each([&entities](const flecs::entity& entity, const auto&) {
    toml::table entity_tbl;
    entity_tbl.emplace("name", entity.name().c_str());
    entity.each([&entity_tbl, entity](const flecs::id& id) {
      serialize_id(entity_tbl, entity, id);
    });

    entities.push_back(entity_tbl);
  });

  scene_tbl.insert("entities", entities);

  out << scene_tbl;

  out.close();

  return {};
}

void serialize_id(toml::table& table, const flecs::entity& entity,
                  const flecs::id& id) {
  if (id.str() == "wren.scene.components.Transform") {
    serialize(table, *entity.get<components::Transform>());
  } else if (id.str() == "wren.scene.components.MeshRenderer") {
    serialize(table, *entity.get<components::MeshRenderer>());
  } else {
    spdlog::info("id.str(): {}", id.str().c_str());
    spdlog::info("id.type_id(): {}", id.type_id().name().c_str());
  }
}

void serialize(toml::table& table,
               const components::MeshRenderer& mesh_renderer) {
  table.insert("mesh_renderer",
               toml::table{{"path", mesh_renderer.mesh_file.string().c_str()}});
}

void serialize(toml::table& table, const components::Transform& transform) {
  toml::table pos;
  serialize(pos, transform.position);

  toml::table rot;
  serialize(rot, transform.rotation);

  toml::table scale;
  serialize(scale, transform.scale);

  toml::table t{
      {"position", pos},
      {"rotation", rot},
      {"scale", scale},
  };
  table.insert("transform", t);
}

void serialize(toml::table& table, const math::Vec3f& vec) {
  table.insert("x", vec.x());
  table.insert("y", vec.y());
  table.insert("z", vec.z());
}

}  // namespace wren::scene
