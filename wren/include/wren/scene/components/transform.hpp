#pragma once

#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <wren_math/geometry.hpp>
#include <wren_math/matrix.hpp>
#include <wren_math/quaternion.hpp>
#include <wren_math/vector.hpp>

namespace wren::scene::components {

struct Transform {
  math::Vec3f position = math::Vec3f(0, 0, -1);
  math::Vec3f rotation{};
  math::Vec3f scale;

  auto matrix() {
    math::Mat4f translate = math::translate(math::Mat4f::identity(), position);
    math::Mat4f rotate = math::Quaternionf(rotation).to_mat();
    // math::Mat4f scale{};

    return translate * rotate;
  }
};

}  // namespace wren::scene::components
