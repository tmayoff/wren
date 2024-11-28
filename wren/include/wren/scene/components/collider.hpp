#pragma once

#include <flecs.h>

#include <wren/math/vector.hpp>
#include <wren/scene/components/transform.hpp>

#include "../components.hpp"

namespace wren::scene::components {

struct Collider : public Base {
  virtual void raycast(const Transform& transform, const math::Vec3f& origin,
                       const math::Vec3f& direction) const = 0;
};

struct BoxCollider2D : public Collider {
  static void init(const flecs::world& world) {
    static bool inited = false;
    if (!inited) {
      inited = true;
      world.component<BoxCollider2D>().is_a<Collider>();
    }
  }

  void raycast(const Transform& transform, const math::Vec3f& origin,
               const math::Vec3f& direction) const override;

  math::Vec2f size;
};

}  // namespace wren::scene::components
