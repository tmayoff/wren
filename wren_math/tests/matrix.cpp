#include <wrenm/matrix.hpp>
#include <wrenm/utils.hpp>

// After utils.hpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Identity") {
  wrenm::mat4f a = wrenm::mat4f::IDENTITY();
  wrenm::mat4f b = wrenm::mat4f::IDENTITY();

  auto c = a * b;
  REQUIRE(c == a);
  REQUIRE(c == b);
}
