#pragma once

#include <boost/describe.hpp>

template <class E> char const *enum_to_string(E e) {
  char const *r = "(unnamed)";
  boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
      [&](auto D) {
        if (e == D.value)
          r = D.name;
      });
  return r;
}

#define DESCRIBED_ENUM(E, ...)                                                 \
  enum class E { __VA_ARGS__ };                                                \
  BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)
