#pragma once

#include <flecs.h>

#include <wren/math/vector.hpp>

namespace wren::scene::components {

struct Collider {};

struct BoxCollider2D : public Collider {
  BoxCollider2D(const flecs::world& world) {
    static bool inited = false;
    if (!inited) {
      inited = true;
      world.component<BoxCollider2D>().is_a<Collider>();
    }
  }

  math::Vec2f size;
};

}  // namespace wren::scene::components
