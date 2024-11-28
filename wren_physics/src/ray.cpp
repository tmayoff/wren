#include "physics/ray.hpp"

#include <wren/scene/components/collider.hpp>
#include <wren/scene/components/transform.hpp>

namespace wren::physics {

auto raycast(const flecs::world& world, const Ray& ray, RayHit& hit) -> bool {
  const auto q = world.query<const scene::components::Transform&,
                             const scene::components::Collider&>();

  q.each([ray, hit](const scene::components::Transform& transform,
                     const scene::components::Collider& collider) {
    // TODO Check raycast
    if (hit.hit) {
      return;
    }

    collider.raycast(transform, ray.origin, ray.direction);
  });

  return false;
}

}  // namespace wren::physics
