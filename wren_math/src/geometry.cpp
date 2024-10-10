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

auto rotate(const Mat3f rotation_matrix) -> Mat4f {
  // return Mat4f{std::array<std::array<float, 4>, 4>{{
  //     {rotation_matrix.data.at(0).at(0), rotation_matrix.data.at(0).at(1),
  //      rotation_matrix.data.at(0).at(2), 0.0f},
  //     {rotation_matrix.data.at(1).at(0), rotation_matrix.data.at(1).at(1),
  //      rotation_matrix.data.at(1).at(2), 0.0f},
  //     {rotation_matrix.data.at(2).at(0), rotation_matrix.data.at(2).at(1),
  //      rotation_matrix.data.at(2).at(2), 0.0f},
  //     {0.0F, 0.0F, 0.0F, 1.0F},
  // }}};

  return Mat4f{};
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

  // mat.data = {{{cos + x2 * (1 - cos), xy * (1 - cos) - z * sin,
  //               xz * (1 - cos) + y * sin, 0},
  //              {yx * (1 - cos) + z * sin, cos + y2 * (1 - cos),
  //               yz * (1 - cos) - x * sin, 0},
  //              {zx * (1 - cos) - y * sin, zy * (1 - cos) + z * sin,
  //               cos + z2 * (1 - cos), 0},
  //              {0, 0, 0, 1}}};

  return mat;
}

}  // namespace wren::math
