#include <boost/test/unit_test.hpp>
#include <wren_math/matrix.hpp>
#include <wren_math/utils.hpp>

BOOST_AUTO_TEST_SUITE(MATRIX)

BOOST_AUTO_TEST_CASE(Identity) {
  std::array<std::array<float, 4>, 4> identity = {
      {
          {1.0F, 0.0F, 0.0F, 0.0F},
          {0.0F, 1.0F, 0.0F, 0.0F},
          {0.0F, 0.0F, 1.0F, 0.0F},
          {0.0F, 0.0F, 0.0F, 1.0F},
      },
  };

  wren::math::Mat4f a = wren::math::Mat4f::identity();
  BOOST_TEST(a.data == identity);

  wren::math::Mat4f b = wren::math::Mat4f::identity();

  const auto c = a * b;
  BOOST_TEST(c == a);
  BOOST_TEST(c == b);
}

BOOST_AUTO_TEST_CASE(Multiplication) {
  // Setup some simple cases

  wren::math::Mat<float, 2> a({{
      {3.0F, 2.0F},
      {-2.0F, 1.0F},
  }});

  wren::math::vec2f b(5.0, 7.0);

  auto c = a * b;

  wren::math::Mat<float, 2> expected({{
      {15.0F, 14.0F},
      {-10.0F, 7.0F},
  }});

  BOOST_TEST(c == expected);
}

BOOST_AUTO_TEST_SUITE_END()
