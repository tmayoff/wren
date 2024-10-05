#include "scene/scene.hpp"

#include "scene/components/tag.hpp"
#include "scene/entity.hpp"

namespace wren::scene {

auto Scene::create_entity(const std::string& name) -> Entity {
  auto entity = Entity{registry_.create(), shared_from_this()};

  entity.add_component<components::Tag>(name);

  return entity;
}

}  // namespace wren::scene
