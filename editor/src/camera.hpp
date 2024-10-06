#pragma once

#include <glm/gtx/transform.hpp>
#include <wren_math/geometry.hpp>
#include <wren_math/matrix.hpp>
#include <wren_math/vector.hpp>

namespace editor {
class Camera {
 public:
  Camera() = default;
  Camera(float fov, float aspect_ratio, float z_near, float z_far)
      : fov_(fov), aspect_ratio_(aspect_ratio), z_near_(z_near), z_far_(z_far) {
    update_projection_matrix();
  }

  static auto create();

  auto aspect(float aspect) {
    aspect_ratio_ = aspect;
    update_projection_matrix();
  }
  [[nodiscard]] auto projection() const { return projection_; }

  auto glm_perspective() const { return glm_projection_; }

 private:
  void update_projection_matrix() {
    projection_ = wren::math::perspective(wren::math::radians(fov_),
                                          aspect_ratio_, z_near_, z_far_);

    glm_projection_ = glm::perspective(wren::math::radians(fov_), aspect_ratio_,
                                       z_near_, z_far_);
  }

  float fov_{};
  float aspect_ratio_{};
  float z_near_{};
  float z_far_{};

  wren::math::Vec3f position_;
  wren::math::Mat4f projection_;
  glm::mat4 glm_projection_;
};
}  // namespace editor
