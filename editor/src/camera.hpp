#pragma once

#include <wren/math/geometry.hpp>
#include <wren/math/matrix.hpp>
#include <wren/math/vector.hpp>
#include <wren/scene/components/transform.hpp>

namespace editor {
class Camera {
 public:
  Camera() = default;
  Camera(float fov, float aspect_ratio, float z_near, float z_far)
      : fov_(fov), aspect_ratio_(aspect_ratio), z_near_(z_near), z_far_(z_far) {
    // transform_.position.z(-5);
    update_projection_matrix();
  }

  static auto create();

  auto aspect(float aspect) {
    aspect_ratio_ = aspect;
    update_projection_matrix();
  }
  [[nodiscard]] auto aspect() const { return aspect_ratio_; }

  [[nodiscard]] auto projection() const { return projection_; }

  [[nodiscard]] auto transform() const { return transform_; }

 private:
  void update_projection_matrix() {
    projection_ = wren::math::perspective(wren::math::radians(fov_),
                                          aspect_ratio_, z_near_, z_far_);
  }

  wren::scene::components::Transform transform_;

  float fov_{};
  float aspect_ratio_{};
  float z_near_{};
  float z_far_{};

  wren::math::Mat4f projection_;
};

}  // namespace editor
