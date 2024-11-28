#pragma once

#include <flecs.h>

#include <wren/math/vector.hpp>

namespace wren::physics {

struct RayHit {
  bool hit = false;
  math::Vec3f point;
};

struct Ray {
  math::Vec3f origin;
  math::Vec3f direction;
};

auto raycast(const flecs::world& world, const Ray& ray, RayHit& hit) -> bool;

}  // namespace wren::physics
