#include "scene/components/collider.hpp"

namespace wren::scene::components {

auto BoxCollider2D::raycast(const Transform& transform,
                            const math::Vec3f& origin,
                            const math::Vec3f& direction) const
    -> std::optional<math::Vec3f> {
  // TODO Ray
  math::Vec3f normal = math::Vec3f{0, 0, 1} * transform.rotation;
  float denominator = normal.dot(direction);
  if (std::abs(denominator) < 0.0001) {
    return {};
  }

  float t = normal.dot(origin) / denominator;
  if (t < 0) {
    // Intersection points is behind the ray's origin
    return {};
  }

  return origin + (direction * t);
}

}  // namespace wren::scene::components
