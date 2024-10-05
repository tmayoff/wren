#pragma once

#include <entt/entt.hpp>
#include <memory>

namespace wren::scene {

class Entity;

class Scene : public std::enable_shared_from_this<Scene> {
 public:
  static auto create() { return std::shared_ptr<Scene>(new Scene()); }

  auto create_entity(const std::string& name = "entity") -> Entity;

  auto registry() const -> const entt::registry& { return registry_; }
  auto registry() -> entt::registry& { return registry_; }

 private:
  Scene() = default;

  entt::registry registry_;
};

}  // namespace wren::scene
