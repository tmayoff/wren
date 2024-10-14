#include "scene/scene.hpp"

#include "scene/components/tag.hpp"
#include "scene/components/transform.hpp"
#include "scene/entity.hpp"

namespace wren::scene {

auto Scene::create_entity(const std::string& name) -> Entity {
  auto entity = Entity{ecs_.entity(name.c_str()), shared_from_this()};

  // entity.add_component<components::Tag>(name);
  entity.add_component<components::Transform>();

  return entity;
}

}  // namespace wren::scene
