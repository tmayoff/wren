#include "scene/serialization.hpp"

#include <spdlog/spdlog.h>

#include <toml++/toml.hpp>

#include "scene/components/tag.hpp"
#include "scene/components/transform.hpp"
#include "scene/entity.hpp"

namespace wren::scene {

void serialize_vec3(toml::table& table, const math::Vec3f& vec);

void serialize_transform(toml::table& table,
                         const components::Transform& transform);

auto serialize(const std::shared_ptr<Scene>& scene,
               const std::filesystem::path& out_file) -> expected<void> {
  auto entities = toml::table{
      {"version", "0.1"},
  };

  // for (const auto& [handle, tag, transform] :
  //      scene->world()
  //          .view<components::Tag, components::Transform>()
  //          .each()) {
  //   toml::table entity_tbl;
  //   entity_tbl.emplace("tag", tag.tag);

  //   serialize_transform(entity_tbl, transform);

  //   // Mesh renderer

  //   entities.emplace(std::to_string(static_cast<uint32_t>(handle)), entity_tbl);
  // }

  // std::ofstream out(out_file);
  // out << entities;

  // out.close();

  return {};
}

void serialize_transform(toml::table& table,
                         const components::Transform& transform) {
  toml::table pos;
  serialize_vec3(pos, transform.position);

  toml::table rot;
  serialize_vec3(rot, transform.rotation);

  toml::table scale;
  serialize_vec3(scale, transform.scale);

  toml::table t{
      {"position", pos},
      {"rotation", rot},
      {"scale", scale},
  };
  table.insert("transform", t);
}

void serialize_vec3(toml::table& table, const math::Vec3f& vec) {
  table.insert("x", vec.x());
  table.insert("y", vec.y());
  table.insert("z", vec.z());
}

}  // namespace wren::scene
