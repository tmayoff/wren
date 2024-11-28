#pragma once

#include <flecs.h>

#include <memory>
#include <optional>
#include <wren/math/vector.hpp>
#include <wren/scene/components/transform.hpp>

#include "../components.hpp"

namespace wren::scene::components {

struct Collider : public Base {
  using Ptr = std::shared_ptr<Collider>;

  [[nodiscard]] virtual auto raycast(const Transform& transform,
                                     const math::Vec3f& origin,
                                     const math::Vec3f& direction) const
      -> std::optional<math::Vec3f> = 0;
};

struct BoxCollider2D : public Collider {
  static void init(const flecs::world& world) {
    static bool inited = false;
    if (!inited) {
      inited = true;
      world.component<BoxCollider2D::Ptr>().is_a<Collider>();
    }
  }

  [[nodiscard]] auto raycast(const Transform& transform,
                             const math::Vec3f& origin,
                             const math::Vec3f& direction) const
      -> std::optional<math::Vec3f> override;

  math::Vec2f size;
};

}  // namespace wren::scene::components
