#include "scene/deserialization.hpp"

#include <spdlog/spdlog.h>

#include <toml++/toml.hpp>

#include "scene/components/mesh.hpp"
#include "scene/components/transform.hpp"
#include "scene/entity.hpp"

namespace wren::scene {

void deserialize(const toml::table& table, components::Transform& transform);
void deserialize(const toml::table& table,
                 const std::filesystem::path& project_root,
                 components::MeshRenderer& mesh_renderer);
void deserialize(const toml::table& table, math::Vec3f& vec);

auto deserialize(const std::filesystem::path& project_root,
                 const std::filesystem::path& file,
                 const std::shared_ptr<Scene>& scene) -> expected<void> {
  const auto table = toml::parse_file((project_root / file).string());

  toml::array entities = *table["entities"].as_array();

  for (const auto& e : entities) {
    const auto& entity_table = *e.as_table();
    auto entity = scene->create_entity(entity_table["name"].as_string()->get());

    for (const auto& [key, val] : entity_table) {
      if (key.str() == "transform") {
        auto& t = entity.get_component<components::Transform>();
        deserialize(*val.as_table(), t);
      } else if (key.str() == "mesh_renderer") {
        components::MeshRenderer mesh_renderer{};
        deserialize(*val.as_table(), project_root, mesh_renderer);
        entity.add_component<components::MeshRenderer>(mesh_renderer);
      }
    }
  }

  return {};
}

void deserialize(const toml::table& table, components::Transform& transform) {
  deserialize(*table["position"].as_table(), transform.position);
  deserialize(*table["rotation"].as_table(), transform.rotation);
  deserialize(*table["scale"].as_table(), transform.scale);
}

void deserialize(const toml::table& table,
                 const std::filesystem::path& project_root,
                 components::MeshRenderer& mesh_renderer) {
  mesh_renderer.update_mesh(project_root, *table["path"].value<std::string>());
}

void deserialize(const toml::table& table, math::Vec3f& vec) {
  vec.x(*table["x"].value<float>());
  vec.y(*table["y"].value<float>());
  vec.z(*table["z"].value<float>());
}

}  // namespace wren::scene
