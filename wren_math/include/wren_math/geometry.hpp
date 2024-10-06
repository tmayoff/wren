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

auto translate(Mat4f mat, Vec4f offset) -> Mat4f;
auto translate(Mat4f mat, Vec3f offset) -> Mat4f;

auto look_at(const Vec3f& position, const Vec3f& target, const Vec3f& world_up)
    -> Mat4f;

auto rotate(const Mat4f& matrix, float rotation, const Vec3f& axis) -> Mat4f;

template <typename T>
inline auto ortho(T left, T right, T bottom, T top) {
  Mat4f res = Mat4f::identity();

  res.data.at(0).at(0) = static_cast<T>(2) / (right - left);
  res.data.at(1).at(1) = static_cast<T>(2) / (top - bottom);
  res.data.at(2).at(2) = -static_cast<T>(1);
  res.data.at(3).at(0) = -(right + left) / (right - left);
  res.data.at(3).at(1) = -(top + bottom) / (top - bottom);

  return res;
}

template <typename T>
inline auto ortho(T left, T right, T bottom, T top, T z_near, T z_far) {
  Mat4f res = Mat4f::identity();

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
  const T tan_half_fovy = std::tan(fov_y / static_cast<T>(2));

  Mat4f res;
  res.data.at(0).at(0) = static_cast<T>(1) / (aspect * tan_half_fovy);
  res.data.at(1).at(1) = static_cast<T>(1) / (tan_half_fovy);
  res.data.at(2).at(2) = -(z_far + z_near) / (z_far - z_near);
  res.data.at(2).at(3) = -static_cast<T>(1);
  res.data.at(3).at(2) =
      -(static_cast<T>(2) * z_far * z_near) / (z_far - z_near);
  res.data.at(3).at(3) = 0;  // FIXME Not sure if this is right here

  return res;
}

}  // namespace wren::math
