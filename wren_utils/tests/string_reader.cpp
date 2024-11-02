#include <boost/test/unit_test.hpp>
#include <wren/utils/string_reader.hpp>

BOOST_AUTO_TEST_SUITE(string_reader)

BOOST_AUTO_TEST_CASE(Read) {
  std::string_view input = R"(
hello world this is tyler
key: value
key1: value1_two
)";

  wren::utils::StringReader reader(input);

  reader.skip_to_text_end("this is ");
  const auto name = reader.read_to_end_line();
  BOOST_TEST(name == "tyler");

  const auto key = reader.read_to_text_start(": ");
  BOOST_TEST(key == "key");
  reader.skip_to_text_end(": ");
  const auto val = reader.read_to_end_line();
  BOOST_TEST(val == "value");

  const auto key1 = reader.read_to_text_start(":");
  BOOST_TEST(key1 == "key1");
  reader.skip_to_text_end(": ");
  const auto val1 = reader.read_to_text_end("_");
  BOOST_TEST(val1 == "value1_");
}

BOOST_AUTO_TEST_SUITE_END();
