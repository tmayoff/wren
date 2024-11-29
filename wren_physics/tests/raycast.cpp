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

  box.add_component<components::BoxCollider2D::Ptr>(
      new components::BoxCollider2D());
  auto collider = std::dynamic_pointer_cast<components::BoxCollider2D>(
      box.get_component<components::BoxCollider2D::Ptr>());

  collider->size.x(1.0);
  collider->size.y(1.0);

  auto& transform = box.get_component<components::Transform>();
  transform.position.z(-10);

  BOOST_TEST(box.has_component<components::BoxCollider2D::Ptr>());

  const auto q = scene->world().query<const components::Collider::Ptr>();
  BOOST_TEST(q.count() == 1);

  physics::Ray ray;
  ray.direction = (transform.position - math::Vec3f(0));

  physics::RayHit hit{};
  physics::raycast(scene->world(), ray, hit);

  BOOST_TEST(hit.hit);

  const float length = (hit.point - math::Vec3f{0, 0, -10}).length();
  BOOST_TEST(length <= 0.001);

  // Move ray
  ray.origin = math::Vec3f{100, 0, 1};

  hit.reset();

  physics::raycast(scene->world(), ray, hit);
  BOOST_TEST(!hit.hit);
}

BOOST_AUTO_TEST_SUITE_END()
