#include "geometry.hpp"

#include <cmath>

#include "matrix.hpp"
#include "vector.hpp"

namespace wren::math {

auto look_at(const Vec3f& eye, const Vec3f& center, const Vec3f& world_up)
    -> Mat4f {
  const Vec3f up = world_up.normalized();
  const Vec3f f = (center - eye).normalized();
  const Vec3f s = (up % f).normalized();
  const Vec3f u = f % s;

  Mat4f mat;

  mat.at(0, 0) = s.x();
  mat.at(1, 0) = s.y();
  mat.at(2, 0) = s.z();
  mat.at(0, 1) = u.x();
  mat.at(1, 1) = u.y();
  mat.at(2, 1) = u.z();
  mat.at(0, 2) = -f.x();
  mat.at(1, 2) = -f.y();
  mat.at(2, 2) = -f.z();
  mat.at(3, 0) = -(s.dot(eye));
  mat.at(3, 1) = -(u.dot(eye));
  mat.at(3, 2) = -(f.dot(eye));
  return mat;
}

auto translate(Mat4f mat, Vec4f offset) -> Mat4f {
  auto res = Mat4f::identity();

  res.at(3, 0) = offset.data.at(0);
  res.at(3, 1) = offset.data.at(1);
  res.at(3, 2) = offset.data.at(2);
  res.at(3, 3) = offset.data.at(3);

  return mat * res;  // * res;
}

auto translate(Mat4f mat, Vec3f offset) -> Mat4f {
  return translate(mat, Vec4f(offset, 1.0));
}

auto rotate(const Mat4f& matrix, float angle, const Vec3f& axis) -> Mat4f {
  const float sin = std::sin(angle);
  const float cos = std::cos(angle);
  const float x2 = axis.x() * axis.x();
  const float y2 = axis.y() * axis.y();
  const float z2 = axis.z() * axis.z();
  const float yx = axis.y() * axis.x();
  const float yz = axis.y() * axis.z();
  const float xy = axis.x() * axis.y();
  const float xz = axis.x() * axis.z();
  const float zx = axis.z() * axis.x();
  const float zy = axis.z() * axis.y();
  const float x = axis.x();
  const float y = axis.y();
  const float z = axis.z();

  Mat4f mat = matrix;
  mat.at(0, 0) = cos + x2 * (1 - cos);
  mat.at(1, 0) = xy * (1 - cos) - z * sin;
  mat.at(2, 0) = xz * (1 - cos) + y * sin;
  mat.at(3, 0) = 0;

  mat.at(0, 1) = yx * (1 - cos) + z * sin;
  mat.at(1, 1) = cos + y2 * (1 - cos);
  mat.at(2, 1) = yz * (1 - cos) - x * sin;
  mat.at(3, 1) = 0.0F;

  mat.at(0, 2) = zx * (1 - cos) - y * sin;
  mat.at(1, 2) = zy * (1 - cos) + z * sin;
  mat.at(2, 2) = cos + z2 * (1 - cos);
  mat.at(3, 0) = 0.0f;

  mat.at(0, 3) = 0.0F;
  mat.at(1, 3) = 0.0f;
  mat.at(2, 3) = 0.0f;
  mat.at(3, 3) = 0.0f;

  return mat;
}

}  // namespace wren::math
