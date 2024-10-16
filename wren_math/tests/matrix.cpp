#include <boost/test/unit_test.hpp>
#include <wren/math/matrix.hpp>
#include <wren/math/utils.hpp>

BOOST_AUTO_TEST_SUITE(MATRIX)

BOOST_AUTO_TEST_CASE(ColMajor) {
  wren::math::Mat<float, 2, 2> a;
  a.at(0, 0) = 1.0;
  a.at(0, 1) = 2.0;
  a.at(1, 0) = 3.0;
  a.at(1, 1) = 4.0;

  BOOST_TEST(a.data().at(0) == 1.0);
  BOOST_TEST(a.data().at(1) == 2.0);
  BOOST_TEST(a.data().at(2) == 3.0);
  BOOST_TEST(a.data().at(3) == 4.0);
}

BOOST_AUTO_TEST_CASE(Identity) {
  std::array<float, 4 * 4> identity = {
      {
          1.0F, 0.0F, 0.0F, 0.0F,  //
          0.0F, 1.0F, 0.0F, 0.0F,  //
          0.0F, 0.0F, 1.0F, 0.0F,  //
          0.0F, 0.0F, 0.0F, 1.0F,  //
      },
  };

  wren::math::Mat4f a = wren::math::Mat4f::identity();
  BOOST_TEST(a.data() == identity);

  wren::math::Mat4f b = wren::math::Mat4f::identity();

  const auto c = a * b;
  BOOST_TEST(c == a);
  BOOST_TEST(c == b);
}

BOOST_AUTO_TEST_CASE(Multiplication_Mat_Mat) {
  wren::math::Mat<float, 2, 2> a{};
  a.at(0, 0) = 3.0f;
  a.at(1, 0) = 7.0f;
  a.at(0, 1) = 4.0f;
  a.at(1, 1) = 9.0f;

  wren::math::Mat<float, 2, 2> b{};
  b.at(0, 0) = 6.0f;
  b.at(1, 0) = 2.0f;
  b.at(0, 1) = 5.0f;
  b.at(1, 1) = 8.0f;

  auto c = a * b;

  wren::math::Mat<float, 2, 2> expected{};
  expected.at(0, 0) = 53.0f;
  expected.at(0, 1) = 69.0f;
  expected.at(1, 0) = 62.0f;
  expected.at(1, 1) = 80.0f;

  BOOST_TEST(c == expected);
}

BOOST_AUTO_TEST_CASE(Multiplication_Mat_Vec) {
  wren::math::Mat<float, 2, 2> a{};
  a.at(0, 0) = 3.0f;
  a.at(0, 1) = -2.0f;
  a.at(1, 0) = 2.0f;
  a.at(1, 1) = 1.0f;

  wren::math::vec2f b(5.0, 7.0);

  auto c = a * b;

  wren::math::vec2f expected(29, -3);

  BOOST_TEST(c == expected);
}

BOOST_AUTO_TEST_SUITE_END()
