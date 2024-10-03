#pragma once

#include <wren_math/vector.hpp>

namespace editor {
class Camera {
 public:
  Camera() = default;

  static auto create();

 private:
  wren::math::vec3f position_;
};
}  // namespace editor
