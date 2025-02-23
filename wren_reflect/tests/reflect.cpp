#include "wren/reflect/reflect.hpp"

#include <boost/test/unit_test.hpp>
#include <editor.hpp>
#include <triangle.hpp>

BOOST_AUTO_TEST_CASE(Triangle) {
  wren::reflect::Reflect reflect{std::as_bytes(std::span(triangle))};

  BOOST_TEST(reflect.magic_number() == 0x07230203);

  const auto entry_points = reflect.entry_points();
  BOOST_TEST(entry_points.size() == 2);
}

BOOST_AUTO_TEST_CASE(Editor) {
  wren::reflect::Reflect reflect{std::as_bytes(std::span(editor))};

  const auto entry_points = reflect.entry_points();
  BOOST_TEST(entry_points.size() == 2);
}
