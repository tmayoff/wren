#pragma once

#include <unordered_map>
#include <vector>

#include "archetype.hpp"
#include "component.hpp"
#include "entity.hpp"

namespace wren::ecs {

struct Archetype;

struct Column {
  void* elements;
  size_t element_size;
  size_t count;
};

struct ArchetypeEdge {
  Archetype& add;
  Archetype& remove;
};

struct Record {
  Archetype& archetype;
  size_t row;
};

// Record in component index with component column for archetype
struct ArchetypeRecord {
  std::size_t column;
};

class Manager {
 public:
  auto create_entity() {
    const auto handle = current_entity_++;
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
  // Find the archetype for an entity
  std::unordered_map<Entity, Archetype&> entity_index_;

  Entity current_entity_ = 0;
};

template <typename T>
auto Manager::has_component(Entity entity) const {
  Archetype& archetype = entity_index_.at(entity);
  // return archetype.type_set.count(get_copmonent_id(T{})) != 0;
}

template <typename T, typename... Args>
auto Manager::add_component(Entity entity, Args&&... args) {
  const auto ptr =
      std::shared_ptr<Component>(new T(std::forward<Args>(args)...));

  // auto& e = entities_.at(entity);
  // if (e.contains(typeid(T))) {
  //   // TODO Return an error
  //   return;
  // }

  // e.insert({typeid(T), ptr});
}

template <typename T>
auto Manager::get_component(Entity entity) const {
  // const auto& e = entities_.at(entity).at(typeid(T));
  // return *e.get();
}

template <typename T>
auto Manager::get_component(Entity entity) -> T& {
  // const auto& e =
  //     std::dynamic_pointer_cast<T>(entities_.at(entity).at(typeid(T)));
  // return *e.get();
}

template <typename... Components>
auto Manager::each() {
  // const auto components = {std::type_index(typeid(Components))...};

  // std::vector<std::tuple<Entity, Components...>> list;

  // std::unordered_map<std::type_index,
  //                    std::vector<std::pair<Entity, Component::Ptr>>>
  //     filtered;

  // // TODO get a filtered list of all entities with all the components
  // requested for (const auto component : components) {
  // }

  // return list;
}

}  // namespace wren::ecs
