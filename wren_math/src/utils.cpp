#include "utils.hpp"

#include "matrix.hpp"

namespace wren::math {

auto operator<<(std::ostream& os, mat4f const& value)
    -> std::ostream& {
  for (std::size_t i = 0; i < value.data.size(); i++) {
    for (std::size_t j = 0; j < value.data.size(); j++) {
      os << value.data.at(i).at(j);
    }
    os << "\n";
  }

  return os;
}

}  // namespace wren::math
