#pragma once

namespace wrenm {

struct vec2f {
  vec2f() : x(0), y(0) {}
  vec2f(float x, float y) : x(x), y(y) {}

  auto operator+(vec2f const& rhs) const {
    return vec2f{x + rhs.x, y + rhs.y};
  }

  auto operator-(vec2f const& rhs) const {
    return vec2f{x - rhs.x, y - rhs.y};
  }

  auto operator==(vec2f const& rhs) const {
    return x == rhs.x && y == rhs.y;
  }

  auto operator!=(vec2f const& rhs) const { return !(*this == rhs); }

  float x;
  float y;
};

struct vec3f {
  vec3f() : x(0), y(0), z(0) {}
  vec3f(float x, float y, float z) : x(x), y(y), z(z) {}

  float x;
  float y;
  float z;
};

}  // namespace wrenm
