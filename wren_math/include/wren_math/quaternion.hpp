#pragma once

#include "matrix.hpp"
#include "vector.hpp"

namespace wren::math {

template <typename T>
struct Quaternion {
  static auto identity() { return Quaternion<T>({0, 0, 0, 1}); }

  Quaternion() : data({0, 0, 0, 1}) {}
  Quaternion(T x, T y, T z, T w) : data({x, y, z, w}) {}

  Quaternion(Vec<T, 3> euler_angles) {
    const auto roll = euler_angles.data.at(0);
    const auto pitch = euler_angles.data.at(1);
    const auto yaw = euler_angles.data.at(2);

    const auto cr = std::cos(roll * 0.5f);
    const auto sr = std::sin(roll * 0.5f);
    const auto cp = std::cos(pitch * 0.5f);
    const auto sp = std::sin(pitch * 0.5f);
    const auto cy = std::cos(yaw * 0.5f);
    const auto sy = std::sin(yaw * 0.5f);

    data.at(0) = sr * cp * cy - cr * sp * sy;
    data.at(1) = cr * sp * cy + sr * cp * sy;
    data.at(2) = cr * cp * sy - sr * sp * cy;
    data.at(3) = cr * cp * cy + sr * sp * sy;
  }

  [[nodiscard]] auto x() const { return data.at(0); }
  auto x(T x) { data.at(0) = x; }
  [[nodiscard]] auto y() const { return data.at(1); }
  [[nodiscard]] auto z() const { return data.at(2); }
  [[nodiscard]] auto w() const { return data.at(3); }

  auto normalize() { data = data.normalized(); }
  auto normalized() const { return Quaternion(data.normalized()); }

  auto to_mat() {
    const auto vec = data.normalized();
    const auto w = vec.at(3);
    const auto x = vec.at(0);
    const auto y = vec.at(1);
    const auto z = vec.at(2);
    // const auto ww = w * w;
    const auto xx = x * x;
    const auto yy = y * y;
    const auto zz = z * z;

    auto result = Mat3f::identity();
    result.at(0, 0) = 1 - 2 * (yy + zz);
    result.at(1, 0) = 2 * (x * y + w * z);
    result.at(2, 0) = 2 * (x * z - w * y);

    result.at(0, 1) = 2 * (x * y - w * z);
    result.at(1, 1) = 1 - 2 * (xx + zz);
    result.at(2, 1) = 2 * (y * z + w * x);

    result.at(0, 2) = 2 * (x * z + w * y);
    result.at(1, 2) = 2 * (y * z - w * x);
    result.at(2, 2) = 1 - 2 * (xx + yy);

    return result;
  }

  Vec<T, 4> data;
};

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

}  // namespace wren::math
