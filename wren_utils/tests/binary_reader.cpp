#include <boost/test/unit_test.hpp>
#include <wren/utils/binray_reader.hpp>

BOOST_AUTO_TEST_SUITE(binary_reader)

BOOST_AUTO_TEST_CASE(Read) {
  std::vector<uint8_t> input = {
      0x53,
      0x53,
      0x53,
      0x53,
      0x53,
      0x53,
      0x53,
      0x53,
      0x53,
      0x53,
      // 1234
      0xD2,
      0x4,
      0x00,
      0x00,

      // 1234
      0xD2,
      0x4,
      0x00,
      0x00,
      // 1234
      0xD2,
      0x4,
      0x00,
      0x00,
      // 1234
      0xD2,
      0x4,
      0x00,
      0x00,
  };

  std::span span(input);

  auto reader = wren::utils::BinaryReader(span);
  reader.skip(10);

  auto num = reader.read<int32_t>();
  BOOST_TEST(num == 1234);

  auto nums = reader.read_list<int32_t, 3>();
  for (const auto n : nums) {
    BOOST_TEST(n == 1234);
  }

  BOOST_TEST(reader.at_end());
}

BOOST_AUTO_TEST_SUITE_END();
