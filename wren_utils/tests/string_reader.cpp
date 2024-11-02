#include <boost/test/unit_test.hpp>
#include <wren/utils/string_reader.hpp>

BOOST_AUTO_TEST_SUITE(string_reader)

BOOST_AUTO_TEST_CASE(Read) {
  std::string_view input = R"(
hello world this is tyler
    
)";

  wren::utils::StringReader reader(input);

  
}

BOOST_AUTO_TEST_SUITE_END();
