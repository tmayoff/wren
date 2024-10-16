#include <boost/test/unit_test.hpp>
#include <wren/math/geometry.hpp>
#include <wren/math/matrix.hpp>
#include <wren/math/utils.hpp>
#include <wren/math/vector.hpp>

BOOST_AUTO_TEST_SUITE(GEOMETRY)

BOOST_AUTO_TEST_CASE(TRANSLATE) {
  struct Test {
    wren::math::Vec3f pos;
    wren::math::Mat4f expected;
  };

  std::array tests = {
      Test{
          {0, 0, 0},
          {},
      },
      Test{
          {0, 1, 0},
          wren::math::Mat4f{wren::math::Mat4f::data_t{{
              // {1, 0, 0, 0},
              // {0, 1, 0, 1},
              // {0, 0, 1, 0},
              // {0, 0, 0, 1},
          }}},
      },
  };

  for (const auto &test : tests) {
    const auto got = wren::math::translate(wren::math::Mat4f{}, test.pos);
    BOOST_TEST(got == test.expected);
  }
}

BOOST_AUTO_TEST_SUITE_END()
