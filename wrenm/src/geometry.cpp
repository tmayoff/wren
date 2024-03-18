#include "geometry.hpp"

#include "matrix.hpp"

namespace wrenm {

auto rotate(wrenm::mat4f const& matrix, float angle,
            wrenm::vec3f const& axis) -> wrenm::mat4f {
  float const sin = std::sin(angle);
  float const cos = std::cos(angle);
  float const x2 = axis.x * axis.x;
  float const y2 = axis.y * axis.y;
  float const z2 = axis.z * axis.z;
  float const yx = axis.y * axis.x;
  float const yz = axis.y * axis.z;
  float const xy = axis.x * axis.y;
  float const xz = axis.x * axis.z;
  float const zx = axis.z * axis.x;
  float const zy = axis.z * axis.y;
  float const x = axis.x;
  float const y = axis.y;
  float const z = axis.z;

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

}  // namespace wrenm
