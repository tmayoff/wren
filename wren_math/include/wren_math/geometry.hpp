#pragma once

#include <numbers>

#include "matrix.hpp"
#include "vector.hpp"

namespace wren::math {

inline auto degrees(float radian) -> float {
  return radian * static_cast<float>(180 / std::numbers::pi);
}

inline auto radians(float degrees) -> float {
  return degrees * static_cast<float>(std::numbers::pi / 180);
}

auto translate(mat4f mat, vec3f offset) -> mat4f;

auto look_at(vec3f const& position, vec3f const& target,
             vec3f const& world_up) -> mat4f;

auto rotate(mat4f const& matrix, float rotation, vec3f const& axis)
    -> mat4f;

template <typename T>
inline auto ortho(T left, T right, T bottom, T top) {
  mat4f res = mat4f::IDENTITY();

  res.data.at(0).at(0) = static_cast<T>(2) / (right - left);
  res.data.at(1).at(1) = static_cast<T>(2) / (top - bottom);
  res.data.at(2).at(2) = -static_cast<T>(1);
  res.data.at(3).at(0) = -(right + left) / (right - left);
  res.data.at(3).at(1) = -(top + bottom) / (top - bottom);

  return res;
}

template <typename T>
inline auto ortho(T left, T right, T bottom, T top, T z_near,
                  T z_far) {
  mat4f res = mat4f::IDENTITY();

  res.data.at(0).at(0) = static_cast<T>(2) / (right - left);
  res.data.at(1).at(1) = static_cast<T>(2) / (top - bottom);
  res.data.at(2).at(2) = static_cast<T>(1) / (z_far - z_near);
  res.data.at(3).at(0) = -(right + left) / (right - left);
  res.data.at(3).at(1) = -(top + bottom) / (top - bottom);
  res.data.at(3).at(2) = -z_near / (z_far - z_near);
  return res;
}

template <typename T>
inline auto perspective(T fov_y, T aspect, T z_near, T z_far) {
  T const tan_half_fovy = std::tan(fov_y / static_cast<T>(2));

  mat4f res;
  res.data.at(0).at(0) = static_cast<T>(1) / (aspect * tan_half_fovy);
  res.data.at(1).at(1) = static_cast<T>(1) / (tan_half_fovy);
  res.data.at(2).at(2) = (z_far + z_near) / (z_far - z_near);
  res.data.at(2).at(3) = static_cast<T>(1);
  res.data.at(3).at(2) =
      (static_cast<T>(2) * z_far * z_near) / (z_far - z_near);

  return res;
}

}  // namespace wren::math
