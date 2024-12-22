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
  // TODO Test these
  res.at(0, 0) = static_cast<T>(2) / (right - left);
  res.at(1, 1) = static_cast<T>(2) / (top - bottom);
  res.at(2, 2) = -static_cast<T>(1);
  res.at(3, 0) = -(right + left) / (right - left);
  res.at(3, 1) = -(top + bottom) / (top - bottom);

  return res;
}

template <typename T>
inline auto ortho(T left, T right, T bottom, T top, T near, T far) {
  Mat4f res = Mat4f::identity();

  res.at(0, 0) = static_cast<T>(2) / (right - left);
  res.at(1, 1) = static_cast<T>(2) / (bottom - top);
  res.at(2, 2) = static_cast<T>(1) / (near - far);
  res.at(3, 0) = -(right + left) / (right - left);
  res.at(3, 1) = -(bottom + top) / (bottom - top);
  res.at(3, 2) = near / (near - far);
  return res;
}

/*
  @brief Create a perspective projection matrix
  @param fov_y Field of view in radians;
*/
template <typename T>
inline auto perspective(T fov_y, T aspect, T z_near, T z_far) {
  const float focal_length = 1.0f / std::tan(fov_y / 2.0f);

  const float x = focal_length / aspect;
  const float y = -focal_length;
  const float a = z_near / (z_far - z_near);
  const float b = z_far * a;

  Mat4f p{};

  p.at(0, 0) = x;
  p.at(1, 1) = y;
  p.at(2, 2) = a;
  p.at(2, 3) = -1.0;
  p.at(3, 2) = b;

  return p;
}

}  // namespace wren::math
