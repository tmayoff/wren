#pragma once

#include <ostream>

#include "matrix.hpp"
#include "vector.hpp"

namespace wren::math {

template <typename T, std::size_t N>
auto operator<<(std::ostream& os, vec<T, N> const& value)
    -> std::ostream& {
  os << "(";
  for (std::size_t i = 0; i < value.data.size(); i++) {
    os << value.data.at(i);
    if (i < value.data.size() - 1) os << ",";
  }
  os << ")";

  return os;
}

auto operator<<(std::ostream& os, mat4f const& value)
    -> std::ostream&;

}  // namespace wren::math
