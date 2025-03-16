#pragma once

#include <wren/math/geometry.hpp>
#include <wren/math/matrix.hpp>
#include <wren/math/quaternion.hpp>
#include <wren/math/vector.hpp>

namespace wren::scene::components {

struct Transform {
  math::Vec3f position = math::Vec3f(0, 0, -1);
  math::Vec3f rotation{};
  math::Vec3f scale;

  [[nodiscard]] auto matrix() const {
    math::Mat4f translate = math::translate(math::Mat4f::identity(), position);
    // math::Mat4f rotate = math::Quaternionf(rotation).to_mat();
    // math::Mat4f scale{};

    return translate;
  }

  [[nodiscard]] auto right() const {
    return (math::Quaternionf{rotation}.to_mat() * math::Vec3f{1, 0, 0})
        .normalized();
  }

  [[nodiscard]] auto up() const {
    return (math::Quaternionf{rotation}.to_mat() * math::Vec3f{0, 1, 0})
        .normalized();
  }
};

}  // namespace wren::scene::components
