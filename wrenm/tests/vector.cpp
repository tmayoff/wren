#include <wrenm/utils.hpp>
#include <wrenm/vector.hpp>

// This needs to be after wrenm/utils.hpp to get the stringization
// functions
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("ADD/SUB") {
  enum class OP { ADD, SUB };

  struct Test {
    wrenm::vec2f a;
    wrenm::vec2f b;

    wrenm::vec2f expected;

    OP op = OP::ADD;
  };

  std::array tests = {
      Test{{5.0f, 5.0f}, {10.0f, 10.0f}, {15.0f, 15.0f}},
      Test{{5.0f, 5.0f}, {10.0f, 10.0f}, {-5.0f, -5.0f}, OP::SUB},
  };

  for (auto const& test : tests) {
    wrenm::vec2f c;
    switch (test.op) {
      case OP::ADD:
        c = test.a + test.b;
        break;
      case OP::SUB:
        c = test.a - test.b;
        break;
    }

    REQUIRE(c == test.expected);
  }
}

TEST_CASE("MUL") {
  struct Test {
    wrenm::vec2f a;
    float scalar = 1;

    wrenm::vec2f expected;
  };

  std::array tests = {
      Test{{5, 5}, 10, {50, 50}},
      Test{{5, 5}, -1, {-5, -5}},
  };

  for (auto const& test : tests) {
    wrenm::vec2f c = test.a * test.scalar;

    REQUIRE(c == test.expected);
  }
}

TEST_CASE("DOT") {
  struct Test {
    wrenm::vec3f a;
    wrenm::vec3f b;
    float expected{};
  };

  std::array tests = {
      Test{{10, 10, 10}, {10, 10, 10}, 1},
      Test{{10, 10, 10}, {15, 15, 15}, 1},
      Test{{10, 10, 10}, {-10, -10, -10}, -1},
  };

  for (auto const& test : tests) {
    auto const got = test.a * test.b;
    REQUIRE_THAT(
        got, Catch::Matchers::WithinAbs(test.expected, 0.0000001));
  }
}
