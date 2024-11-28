#include "scene/components/collider.hpp"

namespace wren::scene::components {

void BoxCollider2D::raycast(const Transform& transform,
                            const math::Vec3f& origin,
                            const math::Vec3f& direction) const {
  // TODO Raycast with plane
}

}  // namespace wren::scene::components
