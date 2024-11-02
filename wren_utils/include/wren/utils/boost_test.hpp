#pragma once

#include <ostream>
#include <type_traits>

#include "enums.hpp"

template <typename T>
  requires std::is_enum_v<T>
auto boost_test_print_type(std::ostream& ostr, const T& right)
    -> std::ostream& {
  ostr << wren::utils::enum_to_string(right);
  return ostr;
}
