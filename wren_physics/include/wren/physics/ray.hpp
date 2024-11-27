#pragma once

namespace wren::physics {

struct RayHit {};

struct Ray {};

auto raycast(const Ray& ray, RayHit& hit) -> bool { return false; }

}  // namespace wren::physics
