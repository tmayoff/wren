#pragma once

#include <wren_math/geometry.hpp>
#include <wren_math/matrix.hpp>
#include <wren_math/vector.hpp>

namespace wren::scene::components {

struct Transform {
  math::Vec3f position;
  math::Vec3f rotation;
  math::Vec3f scale;

  auto matrix() {
    math::Mat4f translate = math::translate(math::Mat4f{}, position);
    math::Mat4f rotate{};
    math::Mat4f scale{};

    return translate * rotate * scale;
  }
};

}  // namespace wren::scene::components
