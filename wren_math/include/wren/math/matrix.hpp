#pragma once

#include <array>

#include "vector.hpp"

namespace wren::math {

/*
 @brief Row major matrix
*/
template <typename T, std::size_t Rows, std::size_t Cols>
class Mat {
 public:
  using mat_t = Mat<T, Rows, Cols>;
  using data_t = std::array<T, Rows * Cols>;

  constexpr static auto identity() {
    mat_t m{};
    for (size_t i = 0; i < Rows; ++i) {
      m.at(i, i) = 1.0;
    }
    return m;
  }

  Mat() = default;
  Mat(const data_t& data) : data_(data) {}
  Mat(const std::array<std::array<T, Cols>, Rows>& rows) {
    for (auto row = 0; row < Rows; ++row) {
      for (auto col = 0; col < Cols; ++col) {
        at(col, row) = rows.at(row).at(col);
      }
    }
  }
  // Mat(float d) { data.fill(std::array<T, N>{d}); }

  auto operator==(const mat_t& rhs) const { return data_ == rhs.data_; }
  auto operator!=(const mat_t& rhs) const { return !(*this == rhs); }
  auto operator*(const mat_t& rhs) {
    mat_t result{};

    // Multiply the rows of this matrix by the cols of rhs

    for (size_t row = 0; row < Cols; ++row) {
      for (size_t col = 0; col < Rows; ++col) {
        result.at(col, row) = 0;
        for (size_t k = 0; k < Cols; ++k) {
          result.at(col, row) += at(k, row) * rhs.at(col, k);
        }
      }
    }
    return result;
  }

  auto operator*(const Vec<T, Rows>& rhs) {
    Vec<T, Rows> res{};

    // Multiply the vector by the columns

    for (auto r = 0; r < Rows; ++r) {
      for (auto c = 0; c < Cols; ++c) {
        res.at(r) += rhs.at(c) * at(c, r);
      }
    }

    return res;
  }

  auto at(std::size_t col, std::size_t row) -> T& {
    return data_.at(get_index(col, row));
  }
  [[nodiscard]] auto at(std::size_t col, std::size_t row) const -> const T& {
    return data_.at(get_index(col, row));
  }

  [[nodiscard]] auto data() const { return data_; }

 private:
  constexpr static auto get_index(std::size_t col, std::size_t row) {
    return col + Cols * row;
  }

  std::array<T, Rows * Cols> data_{};
};

struct Mat4f : public Mat<float, 4, 4> {
  Mat4f() = default;
  Mat4f(const Mat<float, 4, 4>& other) : Mat<float, 4, 4>(other) {}
  Mat4f(const data_t& data) : Mat<float, 4, 4>(data) {}
  Mat4f(const Mat<float, 3, 3>& other) : Mat(identity()) {
    for (auto row = 0; row < 3; ++row) {
      for (auto col = 0; col < 3; ++col) {
        at(col, row) = other.at(col, row);
      }
    }
  }
};

using Mat3f = Mat<float, 3, 3>;

}  // namespace wren::math
