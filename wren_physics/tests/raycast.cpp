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
  struct Test {
    math::Vec3f ray_pos;
    math::Vec3f ray_dir;
    math::Vec3f obj_pos;
    std::optional<math::Vec3f> hit_point;
  };

  std::array tests = {
      Test{
          .ray_pos = {},
          .ray_dir = {0, 0, -1},
          .obj_pos = {0, 0, -10},
          .hit_point = math::Vec3f{0, 0, -10},
      },
      Test{
          .ray_pos = {100, 0, 1},
          .ray_dir = {0, 0, -1},
          .obj_pos = {0, 0, -10},
          .hit_point = {},
      },
      Test{
          .ray_pos = {0, 0, 1},
          .ray_dir = {-0.1, 0.0, -1.0},
          .obj_pos = {0, 0, 0},
          .hit_point = math::Vec3f{-0.1, 0, 0},
      },
  };

  size_t index = 0;
  for (const auto test : tests) {
    BOOST_TEST_INFO_SCOPE(index);

    const auto scene = wren::scene::Scene::create();

    auto box = scene->create_entity("box");

    box.add_component<components::BoxCollider2D::Ptr>(
        new components::BoxCollider2D());
    auto collider = std::dynamic_pointer_cast<components::BoxCollider2D>(
        box.get_component<components::BoxCollider2D::Ptr>());

    collider->size.x(1.0);
    collider->size.y(1.0);

    auto& transform = box.get_component<components::Transform>();
    transform.position = test.obj_pos;

    BOOST_TEST(box.has_component<components::BoxCollider2D::Ptr>());

    const auto q = scene->world().query<const components::Collider::Ptr>();
    BOOST_TEST(q.count() == 1);

    physics::Ray ray;
    ray.origin = test.ray_pos;
    ray.direction = test.ray_dir.normalized();

    physics::RayHit hit{};
    physics::raycast(scene->world(), ray, hit);

    BOOST_TEST(hit.hit == test.hit_point.has_value());

    if (test.hit_point.has_value()) {
      const float length = (hit.point - test.hit_point.value()).length();
      BOOST_TEST(length <= 0.001);
    }

    ++index;
  }
}

BOOST_AUTO_TEST_SUITE_END()
