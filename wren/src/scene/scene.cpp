#include "scene/scene.hpp"

#include "scene/components/tag.hpp"
#include "scene/components/transform.hpp"
#include "scene/entity.hpp"

namespace wren::scene {

auto Scene::create_entity(const std::string& name) -> Entity {
  auto entity = ecs_.entity(name.c_str());

  // entity.add_component<components::Tag>(name);
  entity.add<components::Transform>();

  return {entity, shared_from_this()};
}

}  // namespace wren::scene
