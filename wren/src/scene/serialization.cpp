#include "scene/serialization.hpp"

#include <spdlog/spdlog.h>

#include <toml++/toml.hpp>

#include "scene/components/tag.hpp"

namespace wren::scene {

auto serialize(const Scene& scene, const std::filesystem::path& out_file)
    -> expected<void> {
  auto entities = toml::table{
      {"version", "0.1"},
  };

  for (const auto& [entity, tag] :
       scene.registry().view<components::Tag>().each()) {
    toml::table entity_tbl;
    entity_tbl.emplace("tag", tag.tag);

    entities.emplace(std::to_string(static_cast<uint32_t>(entity)), entity_tbl);
  }

  std::ofstream out(out_file);
  out << entities;

  out.close();

  return {};
}

}  // namespace wren::scene
