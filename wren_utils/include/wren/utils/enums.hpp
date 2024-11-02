#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/describe.hpp>
#include <string>

#define DESCRIBED_ENUM(E, ...)  \
  enum class E { __VA_ARGS__ }; \
  BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)

namespace wren::utils {

template <class E>
auto enum_to_string(E e) -> std::string {
  std::string r = "(unamed)";
  boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
      [&](auto d) {
        if (e == d.value) r = std::string(d.name);
      });
  return r;
}

template <typename E>
auto string_to_enum(const std::string_view& name, bool case_insensitive = false)
    -> std::optional<E> {
  bool found = false;

  E r = {};

  boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
      [&](auto d) {
        const std::string described = d.name;
        if (!found) {
          if (case_insensitive && boost::iequals(name, described)) {
            found = true;
            r = d.value;
          } else if (!case_insensitive && name == described) {
            found = true;
            r = d.value;
          }
        }
      });

  if (found) {
    return r;
  }

  return std::nullopt;
}

}  // namespace wren::utils
