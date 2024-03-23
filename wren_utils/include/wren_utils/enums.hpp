#pragma once

#include <string>
#include <boost/describe.hpp>

template <class E> auto enum_to_string(E e) -> std::string {
  std::string r = "(unnamed)";
  boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
      [&](auto D) {
        if (e == D.value)
          r = std::string(D.name);
      });
  return r;
}

// NOLINTNEXTLINE
#define DESCRIBED_ENUM(E, ...)                                                 \
  enum class E { __VA_ARGS__ };                                                \
  BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)
