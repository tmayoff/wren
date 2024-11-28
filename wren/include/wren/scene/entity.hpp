#pragma once

#include <flecs.h>

#include "components.hpp"
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
  return entity_.has<T>();
}

template <typename T>
auto Entity::get_component() -> T& {
  return *entity_.get_mut<T>();
}

template <typename T>
concept HasInit = requires() { T::init(); };

template <typename T, typename... Args>
void Entity::add_component(Args&&... args) {
  entity_.add<T>();
  if (sizeof...(args) > 0) {
    T t(std::forward<Args...>(args)...);
    entity_.set<T>(t);
  }

  if constexpr (HasInit<T>) {
    T::init(scene_->world());
  }
}

}  // namespace wren::scene
