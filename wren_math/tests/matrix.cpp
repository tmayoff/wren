#include <boost/test/unit_test.hpp>
#include <wren_math/matrix.hpp>
#include <wren_math/utils.hpp>

BOOST_AUTO_TEST_SUITE(MATRIX)

BOOST_AUTO_TEST_CASE(Identity) {
  wren::math::mat4f a = wren::math::mat4f::IDENTITY();
  wren::math::mat4f b = wren::math::mat4f::IDENTITY();

  auto const c = a * b;
  BOOST_TEST(c == a);
  BOOST_TEST(c == b);
}

BOOST_AUTO_TEST_SUITE_END()
