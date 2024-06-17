#include <boost/test/unit_test.hpp>
#include <wren_math/utils.hpp>
#include <wren_math/vector.hpp>

BOOST_AUTO_TEST_SUITE(VECTOR)

BOOST_AUTO_TEST_CASE(ADD_SUB) {
  enum class OP { ADD, SUB };

  struct Test {
    wren::math::vec2f a;
    wren::math::vec2f b;

    wren::math::vec2f expected;

    OP op = OP::ADD;
  };

  std::array tests = {
      Test{{5.0f, 5.0f}, {10.0f, 10.0f}, {15.0f, 15.0f}},
      Test{{5.0f, 5.0f}, {10.0f, 10.0f}, {-5.0f, -5.0f}, OP::SUB},
  };

  for (auto const& test : tests) {
    wren::math::vec2f c;
    switch (test.op) {
      case OP::ADD:
        c = test.a + test.b;
        break;
      case OP::SUB:
        c = test.a - test.b;
        break;
    }

    BOOST_TEST(c == test.expected);
  }
}

BOOST_AUTO_TEST_CASE(MUL) {
  struct Test {
    wren::math::vec2f a;
    float scalar = 1;

    wren::math::vec2f expected;
  };

  std::array tests = {
      Test{{5, 5}, 10, {50, 50}},
      Test{{5, 5}, -1, {-5, -5}},
  };

  for (auto const& test : tests) {
    wren::math::vec2f c = test.a * test.scalar;
    BOOST_TEST(c == test.expected);
  }
}

BOOST_AUTO_TEST_CASE(DOT) {
  struct Test {
    wren::math::vec3f a;
    wren::math::vec3f b;
    float expected = 0;
  };

  std::array tests = {
      Test{{10, 10, 10}, {10, 10, 10}, 1},
      Test{{10, 10, 10}, {15, 15, 15}, 1},
      Test{{10, 10, 10}, {-10, -10, -10}, -1},
  };

  for (auto const& test : tests) {
    float got = test.a.dot(test.b);
    BOOST_TEST(got == test.expected, boost::test_tools::tolerance(0.01F));
  }
}

BOOST_AUTO_TEST_CASE(CROSS) {
  struct Test {
    wren::math::vec3f a;
    wren::math::vec3f b;
    wren::math::vec3f expected{};
  };

  std::array tests = {
      Test{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
      Test{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}},
      Test{{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
  };

  for (auto const& test : tests) {
    auto got = test.a % test.b;
    BOOST_TEST(got == test.expected);
  }
}

BOOST_AUTO_TEST_SUITE_END()
