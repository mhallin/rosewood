#include "rosewood/graphics/camera.h"

#include <ostream>

#include "rosewood/core/transform.h"
#include "rosewood/math/matrix4.h"

using rosewood::math::Matrix4;
using rosewood::math::Vector3;

using rosewood::core::transform;

using rosewood::graphics::Camera;

Matrix4 Camera::projection_matrix() const {
    switch (mode()) {
        case ProjectionMode::kPerspectiveMode:
            return math::make_perspective4(_fov, _aspect, _z_near, _z_far);
        case ProjectionMode::kOrthographicMode:
            return math::make_orthographic(_width, _height, _z_near, _z_far);
    }
}

Matrix4 Camera::inverse_projection_matrix() const {
    switch (mode()) {
        case ProjectionMode::kPerspectiveMode:
            return math::make_inverse_perspective4(_fov, _aspect, _z_near, _z_far);
        case ProjectionMode::kOrthographicMode:
            return math::make_inverse_orthographic(_width, _height, _z_near, _z_far);
    }
}

Vector3 Camera::projection_to_world(Vector3 proj_point) const {
    return transform(entity())->world_transform() * inverse_projection_matrix() * proj_point;
}
