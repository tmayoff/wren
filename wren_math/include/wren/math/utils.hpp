#pragma once

#include <ostream>

#include "matrix.hpp"
#include "vector.hpp"

namespace wren::math {

template <typename T, std::size_t N>
auto operator<<(std::ostream& os, const Vec<T, N>& value) -> std::ostream& {
  os << "(";
  for (std::size_t i = 0; i < value.data.size(); i++) {
    os << value.data.at(i);
    if (i < value.data.size() - 1) os << ",";
  }
  os << ")";

  return os;
}

template <typename T, std::size_t Rows, std::size_t Cols>
auto operator<<(std::ostream& os, const Mat<T, Rows, Cols>& value)
    -> std::ostream& {
  os << "\n(";

  for (std::size_t r = 0; r < Rows; ++r) {
    for (std::size_t c = 0; c < Cols; ++c) {
      os << value.at(c, r);
      if (c < Cols - 1) os << ",";
    }

    if (r < Rows - 1) os << "\n";
  }
  os << ")";
  return os;
}

}  // namespace wren::math
