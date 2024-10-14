#pragma once

#include <flecs.h>

#include <memory>

namespace wren::scene {

class Entity;

class Scene : public std::enable_shared_from_this<Scene> {
 public:
  static auto create() { return std::shared_ptr<Scene>(new Scene()); }

  auto create_entity(const std::string& name = "entity") -> Entity;

  auto world() const -> const flecs::world& { return ecs_; }
  auto world() -> flecs::world& { return ecs_; }

 private:
  Scene() = default;

  flecs::world ecs_;
};

}  // namespace wren::scene
