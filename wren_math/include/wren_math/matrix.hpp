#pragma once

#include <array>

#include "vector.hpp"

namespace wren::math {

// ROW First matrices
template <typename T, std::size_t N>
class Mat {
 public:
  using mat_t = Mat<T, N>;

  constexpr static auto identity() {
    std::array<std::array<T, N>, N> data{};

    for (size_t i = 0; i < N; ++i) {
      data.at(i).at(i) = 1.0;
    }
    return Mat(data);
  }

  Mat() : data(identity().data) {}
  Mat(const std::array<std::array<T, N>, N>& data) : data(data) {}
  Mat(float d) { data.fill(std::array<T, N>{d}); }

  auto operator==(const Mat<T, N>& rhs) const { return data == rhs.data; }
  auto operator!=(const Mat<T, N>& rhs) const { return !(*this == rhs); }
  auto operator*(const Mat<T, N>& rhs) {
    Mat<T, N> m;

    for (std::size_t i = 0; i < data.size(); i++) {
      for (std::size_t j = 0; j < rhs.data.at(i).size(); j++) {
        float sum = 0;
        for (std::size_t k = 0; k < rhs.data.size(); k++) {
          sum += data.at(i).at(k) * rhs.data.at(k).at(j);
        }
        m.data.at(i).at(j) = sum;
      }
    }

    return m;
  }

  auto operator*(const Vec<T, N>& rhs) {
    Mat<T, N> res{};

    for (size_t v = 0; v < N; ++v) {
      for (size_t m = 0; m < N; ++m) {
        res.data.at(m).at(v) = data.at(m).at(v) * rhs.data.at(v);
      }
    }

    return res;
  }

  std::array<std::array<T, N>, N> data{};

 private:
  auto columns() {}
};

struct Mat4f : Mat<float, 4> {
  Mat4f() = default;
  Mat4f(const Mat<float, 4>& other) : Mat<float, 4>(other) {}
};

}  // namespace wren::math
