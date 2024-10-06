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

template <typename T, std::size_t N>
auto operator<<(std::ostream& os, const Mat<T, N>& value) -> std::ostream& {
  os << "\n(";

  for (std::size_t r = 0; r < N; ++r) {
    for (std::size_t c = 0; c < N; ++c) {
      os << value.data.at(r).at(c);
      if (c < N - 1) os << ",";
    }

    if (r < N - 1) os << "\n";
  }
  os << ")";
  return os;
}

}  // namespace wren::math
