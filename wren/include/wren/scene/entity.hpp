#pragma once

#include <flecs.h>

#include "scene.hpp"

namespace wren::scene {

class Scene;

class Entity {
 public:
  Entity(flecs::entity entity, const std::shared_ptr<Scene>& scene)
      : entity_(entity), scene_(scene) {}

  template <typename T>
  [[nodiscard]] auto has_component() const -> bool;

  template <typename T>
  auto get_component() -> T&;

  template <typename T, typename... Args>
  void add_component(Args&&... args);

 private:
  flecs::entity entity_;

  std::shared_ptr<Scene> scene_;
};

template <typename T>
auto Entity::has_component() const -> bool {
  // return scene_->world().all_of<T>();
}

template <typename T>
auto Entity::get_component() -> T& {
  return scene_->world().get<T>(entity_);
}

template <typename T, typename... Args>
void Entity::add_component(Args&&... args) {
  entity_.add<T>();
  entity_.set<T>(T(std::forward<Args>(args)...));
}

}  // namespace wren::scene
