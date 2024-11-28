#include <boost/test/unit_test.hpp>
#include <wren/physics/ray.hpp>
#include <wren/scene/components.hpp>
#include <wren/scene/components/collider.hpp>
#include <wren/scene/components/transform.hpp>
#include <wren/scene/entity.hpp>
#include <wren/scene/scene.hpp>

namespace physics = wren::physics;
namespace components = wren::scene::components;
namespace math = wren::math;

BOOST_AUTO_TEST_SUITE(raycasting)

BOOST_AUTO_TEST_CASE(RaycastBoxCollider) {
  const auto scene = wren::scene::Scene::create();

  auto box = scene->create_entity("box");

  box.add_component<components::BoxCollider2D>();
  auto& transform = box.get_component<components::Transform>();
  transform.position.z(-10);

  BOOST_TEST(box.has_component<components::BoxCollider2D>());

  const auto q = scene->world().query<components::Collider>();
  BOOST_TEST(q.count() > 0);

  physics::Ray ray;
  ray.direction = (math::Vec3f(0) - transform.position);

  physics::RayHit hit{};
  // physics::raycast(scene->world(), ray, hit);

  // BOOST_TEST(hit.hit);
}

BOOST_AUTO_TEST_SUITE_END()
