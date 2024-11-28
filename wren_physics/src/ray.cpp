#include "physics/ray.hpp"

#include <wren/scene/components/collider.hpp>
#include <wren/scene/components/transform.hpp>

namespace wren::physics {

auto raycast(const flecs::world& world, const Ray& ray, RayHit& hit) -> bool {
  const auto q = world.query<const scene::components::Transform,
                             const scene::components::Collider::Ptr>();

  q.each([ray, &hit](const scene::components::Transform& transform,
                     const scene::components::Collider::Ptr& collider) {
    if (hit.hit) {
      return;
    }

    auto pos = collider->raycast(transform, ray.origin, ray.direction);
    if (pos.has_value()) {
      hit.hit = true;
    }
  });

  return hit.hit;
}

}  // namespace wren::physics
