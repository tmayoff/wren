
#include <gsl/gsl-lite.hpp>
#include <wrenm/utils.hpp>
#include <wrenm/vector.hpp>

//
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple arithmetic") {
  enum class OP { ADD, SUB, DIV, MUL };

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
      case OP::DIV:
      case OP::MUL:
        break;
    }

    REQUIRE(c == test.expected);
  }
}
