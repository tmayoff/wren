#include "geometry.hpp"

#include <cmath>

#include "matrix.hpp"
#include "vector.hpp"

namespace wren::math {

auto look_at(vec3f const& eye, vec3f const& center,
             vec3f const& world_up) -> mat4f {
  vec3f const up = world_up.normalized();
  vec3f const f = (center - eye).normalized();
  vec3f const s = (up % f).normalized();
  vec3f const u = f % s;

  mat4f mat;

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

auto translate(mat4f mat, vec3f offset) -> mat4f {
  return mat4f::IDENTITY();
}

auto rotate(mat4f const& matrix, float angle,
            vec3f const& axis) -> mat4f {
  float const sin = std::sin(angle);
  float const cos = std::cos(angle);
  float const x2 = axis.x() * axis.x();
  float const y2 = axis.y() * axis.y();
  float const z2 = axis.z() * axis.z();
  float const yx = axis.y() * axis.x();
  float const yz = axis.y() * axis.z();
  float const xy = axis.x() * axis.y();
  float const xz = axis.x() * axis.z();
  float const zx = axis.z() * axis.x();
  float const zy = axis.z() * axis.y();
  float const x = axis.x();
  float const y = axis.y();
  float const z = axis.z();

  mat4f mat = matrix;

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
