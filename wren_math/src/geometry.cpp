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

  mat.data.at(0).at(0) = s.x();
  mat.data.at(1).at(0) = s.y();
  mat.data.at(2).at(0) = s.z();
  mat.data.at(0).at(1) = u.x();
  mat.data.at(1).at(1) = u.y();
  mat.data.at(2).at(1) = u.z();
  mat.data.at(0).at(2) = -f.x();
  mat.data.at(1).at(2) = -f.y();
  mat.data.at(2).at(2) = -f.z();
  mat.data.at(3).at(0) = -(s.dot(eye));
  mat.data.at(3).at(1) = -(u.dot(eye));
  mat.data.at(3).at(2) = -(f.dot(eye));
  return mat;
}

auto translate(Mat4f mat, Vec4f offset) -> Mat4f {
  auto res = Mat4f::identity();

  for (size_t i = 0; i < 4; ++i) {
    res.data.at(3).at(i) = offset.data.at(i);
  }

  return res;
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

  mat.data = {{{cos + x2 * (1 - cos), xy * (1 - cos) - z * sin,
                xz * (1 - cos) + y * sin, 0},
               {yx * (1 - cos) + z * sin, cos + y2 * (1 - cos),
                yz * (1 - cos) - x * sin, 0},
               {zx * (1 - cos) - y * sin, zy * (1 - cos) + z * sin,
                cos + z2 * (1 - cos), 0},
               {0, 0, 0, 1}}};

  return mat;
}

}  // namespace wren::math
