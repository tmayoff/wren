#include <boost/test/unit_test.hpp>
#include <wren/math/matrix.hpp>
#include <wren/math/utils.hpp>

namespace wm = wren::math;

BOOST_AUTO_TEST_CASE(RowMajorIdentity) {
  wm::Mat4f identity = wm::Mat4f::identity();

  const auto data = identity.data();

  const std::array expected = {
      1, 0, 0, 0,  // Row 1
      0, 1, 0, 0,  // Row 2
      0, 0, 1, 0,  // Row 3
      0, 0, 0, 1,
  };

  BOOST_TEST(data == expected, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(RowMajor) {
  wren::math::Mat<float, 2, 2> a;
  a.at(0, 0) = 1.0;
  a.at(1, 0) = 3.0;
  a.at(0, 1) = 2.0;
  a.at(1, 1) = 4.0;

  const std::array expected = {
      1.0, 3.0,  // Row 1
      2.0, 4.0,  // Row 2
  };

  BOOST_TEST(a.data() == expected, boost::test_tools::per_element());
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

BOOST_AUTO_TEST_CASE(MultiplicationMatMat) {
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

BOOST_AUTO_TEST_CASE(MultiplicationMatVec) {
  wren::math::Mat<float, 2, 2> a{};
  a.at(0, 0) = 3.0f;
  a.at(0, 1) = -2.0f;
  a.at(1, 0) = 2.0f;
  a.at(1, 1) = 1.0f;

  wren::math::Vec2f b(5.0, 7.0);

  auto c = a * b;

  wren::math::Vec2f expected(29, -3);

  BOOST_TEST(c == expected);
}
