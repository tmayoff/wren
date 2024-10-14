#include <boost/test/unit_test.hpp>
#include <wren/ecs/manager.hpp>

class Test : public wren::ecs::Component {
 public:
  Test(int32_t index) : index_(index) {}

  [[nodiscard]] auto index() const { return index_; }
  auto index(int32_t i) { index_ = i; }

 private:
  int32_t index_;
};

BOOST_AUTO_TEST_SUITE(ECS)

// BOOST_AUTO_TEST_CASE(CREATE) {
//   wren::ecs::Manager m;

//   auto handle = m.create_entity();

//   BOOST_TEST(handle == 0);
// }

// BOOST_AUTO_TEST_CASE(AddComponent) {
//   wren::ecs::Manager m;

//   const auto handle = m.create_entity();

//   m.add_component<Test>(handle, 0);

//   BOOST_TEST(m.has_component<Test>(handle));
// }

// BOOST_AUTO_TEST_CASE(GetComponent) {
//   wren::ecs::Manager m;

//   const auto handle = m.create_entity();

//   m.add_component<Test>(handle, 123);

//   BOOST_TEST(m.has_component<Test>(handle));

//   auto& test = m.get_component<Test>(handle);
//   BOOST_TEST(test.index() == 123);

//   test.index(321);

//   test = m.get_component<Test>(handle);
//   BOOST_TEST(test.index() == 321);
// }

// BOOST_AUTO_TEST_CASE(Iterate) {
//   wren::ecs::Manager m;

//   for (size_t i = 0; i < 100; ++i) {
//     const auto handle = m.create_entity();
//     m.add_component<Test>(handle, i);
//   }

//   auto list = m.each<Test>();

//   BOOST_TEST(list.size() == 100);
// }

BOOST_AUTO_TEST_SUITE_END();
