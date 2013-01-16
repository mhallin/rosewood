#include "rosewood/graphics/camera.h"

#include <ostream>

#include "rosewood/math/matrix4.h"

using rosewood::math::Matrix4;

using rosewood::graphics::Camera;

Matrix4 Camera::projection_matrix() const {
    return math::make_perspective(_fov, _aspect, _z_near, _z_far);
}

Matrix4 Camera::inverse_projection_matrix() const {
    return math::make_inverse_perspective(_fov, _aspect, _z_near, _z_far);
}
