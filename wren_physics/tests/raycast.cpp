#include <boost/test/unit_test.hpp>
#include <wren/scene/components.hpp>
#include <wren/scene/components/collider.hpp>
#include <wren/scene/entity.hpp>
#include <wren/scene/scene.hpp>

BOOST_AUTO_TEST_SUITE(raycasting)

BOOST_AUTO_TEST_CASE(RaycastBoxCollider) {
  const auto scene = wren::scene::Scene::create();

  auto box = scene->create_entity("box");

  box.add_component<wren::scene::components::BoxCollider2D>();

  BOOST_TEST(box.has_component<wren::scene::components::BoxCollider2D>());

  const auto q = scene->world().query<wren::scene::components::Collider>();
  BOOST_TEST(q.count() > 0);
}

BOOST_AUTO_TEST_SUITE_END()
