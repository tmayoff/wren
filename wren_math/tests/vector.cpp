#include <boost/test/unit_test.hpp>
#include <wren/math/utils.hpp>
#include <wren/math/vector.hpp>

BOOST_AUTO_TEST_SUITE(vector_test_suite)

BOOST_AUTO_TEST_CASE(AddSub) {
  enum class OP { Add, Sub };

  struct Test {
    wren::math::Vec2f a;
    wren::math::Vec2f b;

    wren::math::Vec2f expected;

    OP op = OP::Add;
  };

  std::array tests = {
      Test{
          .a = {5.0f, 5.0f},
          .b = {10.0f, 10.0f},
          .expected = {15.0f, 15.0f},
      },
      Test{.a = {5.0f, 5.0f},
           .b = {10.0f, 10.0f},
           .expected = {-5.0f, -5.0f},
           .op = OP::Sub},
  };

  for (const auto& test : tests) {
    wren::math::Vec2f c;
    switch (test.op) {
      case OP::Add:
        c = test.a + test.b;
        break;
      case OP::Sub:
        c = test.a - test.b;
        break;
    }

    BOOST_TEST(c == test.expected);
  }
}

BOOST_AUTO_TEST_CASE(MUL) {
  struct Test {
    wren::math::Vec2f a;
    float scalar = 1;

    wren::math::Vec2f expected;
  };

  std::array tests = {
      Test{
          .a = {5, 5},
          .scalar = 10,
          .expected = {50, 50},
      },
      Test{
          .a = {5, 5},
          .scalar = -1,
          .expected = {-5, -5},
      },
  };

  for (const auto& test : tests) {
    wren::math::Vec2f c = test.a * test.scalar;
    BOOST_TEST(c == test.expected);
  }
}

BOOST_AUTO_TEST_CASE(DOT) {
  struct Test {
    wren::math::Vec3f a;
    wren::math::Vec3f b;
    float expected = 0;
  };

  std::array tests = {
      Test{.a = {0.5, 0.5, 0}, .b = {0.5, 0.5, 0}, .expected = 0.5},
      Test{.a = {1, 1, 0}, .b = {1, 1, 0}, .expected = 2},
      Test{.a = {0.5, 0.5, 0}, .b = {-0.5, -0.5, 0}, .expected = -0.5},
  };

  for (const auto& test : tests) {
    float got = test.a.dot(test.b);
    BOOST_TEST(got == test.expected, boost::test_tools::tolerance(0.01F));
  }
}

BOOST_AUTO_TEST_CASE(CROSS) {
  struct Test {
    wren::math::Vec3f a;
    wren::math::Vec3f b;
    wren::math::Vec3f expected{};
  };

  std::array tests = {
      Test{.a = {1, 0, 0}, .b = {0, 1, 0}, .expected = {0, 0, 1}},
      Test{.a = {0, 1, 0}, .b = {0, 0, 1}, .expected = {1, 0, 0}},
      Test{.a = {0, 0, 1}, .b = {1, 0, 0}, .expected = {0, 1, 0}},
  };

  for (const auto& test : tests) {
    auto got = test.a % test.b;
    BOOST_TEST(got == test.expected);
  }
}

BOOST_AUTO_TEST_SUITE_END()
