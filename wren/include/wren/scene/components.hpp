#pragma once

#include <flecs.h>

namespace wren::scene::components {

struct Base {
  Base() = default;
  Base(const Base &) = default;
  Base(Base &&) = delete;
  auto operator=(const Base &) -> Base & = default;
  auto operator=(Base &&) -> Base & = delete;
  virtual ~Base() = default;

  // virtual void init(const flecs::world &world) = 0;
};

// void iterate_known_components(const Scene& scene, entt::entity entity,
//                               std::function<void()>);

}  // namespace wren::scene::components
