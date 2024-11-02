#include <boost/test/unit_test.hpp>
#include <wren/utils/boost_test.hpp>
#include <wren/utils/enums.hpp>

DESCRIBED_ENUM(TestEnums, Case1, Case2);

BOOST_AUTO_TEST_SUITE(enums)

BOOST_AUTO_TEST_CASE(EnumToString) {
  BOOST_TEST(wren::utils::enum_to_string(TestEnums::Case1) == "Case1");
  BOOST_TEST(wren::utils::enum_to_string(TestEnums::Case2) == "Case2");
}

BOOST_AUTO_TEST_CASE(StringToEnumCaseSensitive) {
  auto res = wren::utils::string_to_enum<TestEnums>("case1");
  BOOST_TEST(!res.has_value());

  res = wren::utils::string_to_enum<TestEnums>("Case1");
  BOOST_TEST(res.has_value());
  BOOST_TEST(res.value() == TestEnums::Case1);
}

BOOST_AUTO_TEST_CASE(StringToEnumCaseInsensitive) {
  auto res = wren::utils::string_to_enum<TestEnums>("case2", true);
  BOOST_TEST(res.has_value());
  BOOST_TEST(res.value() == TestEnums::Case2);

  res = wren::utils::string_to_enum<TestEnums>("case1", true);
  BOOST_TEST(res.has_value());
  BOOST_TEST(res.value() == TestEnums::Case1);
}

BOOST_AUTO_TEST_SUITE_END()
