#include "scene/scene.hpp"

#include "scene/components/tag.hpp"
#include "scene/components/transform.hpp"
#include "scene/entity.hpp"

namespace wren::scene {

auto Scene::create_entity(const std::string& name) -> flecs::entity {
  auto entity = ecs_.entity(name.c_str());

  // entity.add_component<components::Tag>(name);
  entity.add<components::Transform>();

  return entity;
}

}  // namespace wren::scene
