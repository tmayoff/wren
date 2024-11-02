#include <boost/test/unit_test.hpp>
#include <wren/utils/binray_reader.hpp>

BOOST_AUTO_TEST_SUITE(binary_reader)

BOOST_AUTO_TEST_CASE(Read) {
  std::vector<uint8_t> input = {
      0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53,
  };

  std::span span(input);

  auto reader = wren::utils::BinaryReader(span);
  reader.skip(10);

  BOOST_TEST(reader.at_end());
}

BOOST_AUTO_TEST_SUITE_END();
