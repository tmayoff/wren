#pragma once

#include <entt/entt.hpp>

#include "scene.hpp"

namespace wren::scene {

class Scene;

class Entity {
 public:
  Entity(entt::entity entity, const std::shared_ptr<Scene>& scene)
      : entity_(entity), scene_(scene) {}

  template <typename T>
  [[nodiscard]] auto has_component() const -> bool;

  template <typename T>
  auto get_component() -> T&;

  template <typename T, typename... Args>
  void add_component(Args&&... args);

 private:
  entt::entity entity_;

  std::shared_ptr<Scene> scene_;
};

template <typename T>
auto Entity::has_component() const -> bool {
  return scene_->registry().all_of<T>();
}

template <typename T>
auto Entity::get_component() -> T& {
  return scene_->registry().get<T>(entity_);
}

template <typename T, typename... Args>
void Entity::add_component(Args&&... args) {
  scene_->registry().emplace<T>(entity_, std::forward<Args>(args)...);
}

}  // namespace wren::scene
