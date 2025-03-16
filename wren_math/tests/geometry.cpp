#include <boost/test/unit_test.hpp>
#include <wren/math/geometry.hpp>
#include <wren/math/matrix.hpp>
#include <wren/math/utils.hpp>
#include <wren/math/vector.hpp>

namespace wm = wren::math;

BOOST_AUTO_TEST_CASE(TRANSLATE) {
  struct Test {
    wren::math::Vec3f pos;
    wren::math::Mat4f expected;
  };

  std::array tests = {
      Test{
          .pos = {0, 0, 0},
          .expected = {},
      },
      Test{
          .pos = {0, 1, 0},
          .expected = wm::Mat4f{wm::Mat4f::data_t{{
              // {1, 0, 0, 0},
              // {0, 1, 0, 1},
              // {0, 0, 1, 0},
              // {0, 0, 0, 1},
          }}},
      },
  };

  for (const auto &test : tests) {
    const auto got = wm::translate(wm::Mat4f{}, test.pos);
    BOOST_TEST(got == test.expected);
  }
}
