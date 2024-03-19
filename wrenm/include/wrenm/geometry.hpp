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

auto translate(mat4f mat, vec3f offset) -> mat4f;

auto look_at(wrenm::vec3f const& position, wrenm::vec3f const& target,
             wrenm::vec3f const& world_up) -> wrenm::mat4f;

auto rotate(wrenm::mat4f const& matrix, float rotation,
            wrenm::vec3f const& axis) -> wrenm::mat4f;

}  // namespace wrenm
