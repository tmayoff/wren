#pragma once

#include <numbers>

#include "matrix.hpp"
#include "vector.hpp"

namespace wrenm {

inline auto degrees(float radian) -> float {
  return radian * static_cast<float>(180 / std::numbers::pi);
}

inline auto radians(float degrees) -> float {
  return degrees * static_cast<float>(std::numbers::pi / 180);
}

auto rotate(wrenm::mat4f const& matrix, float rotation,
            wrenm::vec3f const& axis) -> wrenm::mat4f;

}  // namespace wrenm
