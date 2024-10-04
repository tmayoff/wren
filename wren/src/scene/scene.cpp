#include "scene/scene.hpp"

#include "scene/entity.hpp"

namespace wren::scene {

auto Scene::create_entity() -> Entity {
  auto entity = Entity{registry_.create(), shared_from_this()};

  return entity;
}

}  // namespace wren::scene
