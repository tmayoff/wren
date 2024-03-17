#include <wrenm/utils.hpp>
#include <wrenm/vector.hpp>

// This needs to be after wrenm/utils.hpp to get the stringization
// functions
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ADD/SUB") {
  enum class OP { ADD, SUB };

  struct Test {
    wrenm::vec2f a;
    wrenm::vec2f b;

    wrenm::vec2f expected;

    OP op = OP::ADD;
  };

  std::array tests = {
      Test{{5, 5}, {10, 10}, {15, 15}},
      Test{{5, 5}, {10, 10}, {-5, -5}, OP::SUB},
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
  enum class OP { ADD, SUB };

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
    wrenm::vec2f c;
    c = test.a * test.scalar;

    REQUIRE(c == test.expected);
  }
}
