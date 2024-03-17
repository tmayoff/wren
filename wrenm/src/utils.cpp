#include "wrenm/utils.hpp"

#include "wrenm/vector.hpp"

auto operator<<(std::ostream& os, wrenm::vec2f const& value)
    -> std::ostream& {
  os << "(" << value.x << ", " << value.y << ")";
  return os;
}
