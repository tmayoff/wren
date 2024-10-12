#pragma once

#include <typeindex>
#include <unordered_map>
#include <vector>

#include "component.hpp"
#include "entity.hpp"

namespace wren::ecs {

class Manager {
 public:
  auto create_entity() {
    const auto handle = current_entity_++;
    entities_.insert({handle, {}});

    return handle;
  }

  template <typename T>
  auto has_component(Entity entity) const;

  template <typename T, typename... Args>
  auto add_component(Entity entity, Args&&... args);

  template <typename T>
  auto get_component(Entity entity) const;

  template <typename T>
  auto get_component(Entity entity) -> T&;

  template <typename... Components>
  auto each();

 private:
  std::unordered_map<Entity,
                     std::unordered_map<std::type_index, Component::Ptr>>
      entities_;

  std::unordered_map<std::type_index,
                     std::vector<std::pair<Entity, Component::Ptr>>>
      components_;

  Entity current_entity_ = 0;
};

template <typename T>
auto Manager::has_component(Entity entity) const {
  const auto& e = entities_.at(entity);
  return e.contains(typeid(T));
}

template <typename T, typename... Args>
auto Manager::add_component(Entity entity, Args&&... args) {
  const auto ptr =
      std::shared_ptr<Component>(new T(std::forward<Args>(args)...));

  auto& e = entities_.at(entity);
  if (e.contains(typeid(T))) {
    // TODO Return an error
    return;
  }

  e.insert({typeid(T), ptr});
}

template <typename T>
auto Manager::get_component(Entity entity) const {
  const auto& e = entities_.at(entity).at(typeid(T));
  return *e.get();
}

template <typename T>
auto Manager::get_component(Entity entity) -> T& {
  const auto& e =
      std::dynamic_pointer_cast<T>(entities_.at(entity).at(typeid(T)));
  return *e.get();
}

template <typename... Components>
auto Manager::each() {
  const auto components = {std::type_index(typeid(Components))...};

  std::vector<std::tuple<Entity, Components...>> list;

  std::unordered_map<std::type_index,
                     std::vector<std::pair<Entity, Component::Ptr>>>
      filtered;

  // TODO get a filtered list of all entities with all the components requested
  for (const auto component : components) {
  }

  return list;
}

}  // namespace wren::ecs
