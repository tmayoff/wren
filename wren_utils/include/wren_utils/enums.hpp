#pragma once

#include <boost/describe.hpp>
#include <string>

template <class E>
auto enum_to_string(E e) -> std::string {
  std::string r = "(unamed)";
  boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
      [&](auto D) {
        if (e == D.value) r = std::string(D.name);
      });
  return r;
}

// NOLINTNEXTLINE
#define DESCRIBED_ENUM(E, ...)  \
  enum class E { __VA_ARGS__ }; \
  BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)
