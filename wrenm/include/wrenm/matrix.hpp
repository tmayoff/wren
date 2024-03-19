#pragma once

#include <array>

namespace wrenm {

struct mat4f {
  using mat_t = std::array<std::array<float, 4>, 4>;

  static auto IDENTITY() {
    return mat4f{{{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }}};
  }

  mat4f() { data = IDENTITY().data; }
  mat4f(float d) { data.fill(std::array<float, 4>{d}); }
  mat4f(mat_t const& data) : data(data) {}

  auto operator==(mat4f const& rhs) const { return data == rhs.data; }
  auto operator!=(mat4f const& rhs) const { return !(*this == rhs); }

  auto operator*(mat4f const& rhs) {
    mat4f m;

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

  mat_t data{};
};

}  // namespace wrenm
