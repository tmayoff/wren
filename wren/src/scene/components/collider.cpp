#include "scene/components/collider.hpp"

namespace wren::scene::components {

auto BoxCollider2D::raycast(const Transform& transform,
                            const math::Vec3f& origin,
                            const math::Vec3f& direction) const
    -> std::optional<math::Vec3f> {
  math::Vec3f normal =
      math::Quaternionf{transform.rotation}.to_mat() * math::Vec3f{0, 0, 1};

  float denominator = normal.dot(direction);
  if (std::abs(denominator) < 1e-6) {
    return {};
  }

  // Calculate the position along the ray
  float t = (transform.position - origin).dot(normal) / denominator;
  if (t < 0) {
    // Intersection points is behind the ray's origin
    return {};
  }

  const auto point = origin + direction * t;
  math::Vec3f projected_point = point - transform.position;
  // projected_point -= projected_point.dot(normal) * normal;

  const math::Vec3f left_edge = -(transform.right() * size.x() / 2);
  const math::Vec3f right_edge = (transform.right() * size.x() / 2);
  const math::Vec3f top_edge = (transform.up() * size.y() / 2);
  const math::Vec3f bottom_edge = -(transform.up() * size.y() / 2);

  if (projected_point.x() > left_edge.x() &&
      projected_point.x() < right_edge.x() &&
      projected_point.y() < top_edge.y() &&
      projected_point.y() > bottom_edge.y()) {
    return point;
  }

  return {};
}

}  // namespace wren::scene::components
