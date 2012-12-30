#include "rosewood/graphics/camera.h"

#include <ostream>

#include "rosewood/math/matrix4.h"

using rosewood::math::Matrix4;

using rosewood::graphics::Camera;

Camera::~Camera() { }

Matrix4 Camera::projection_matrix() const {
    return math::make_perspective(_fov, _aspect, _z_near, _z_far);
}

Matrix4 Camera::inverse_projection_matrix() const {
    return math::make_inverse_perspective(_fov, _aspect, _z_near, _z_far);
}

void Camera::print_debug_info(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "- Camera: " << this << "\n";
    os << std::string(indent, ' ') << "  FOV: " << _fov << ", Aspect: " << _aspect << ", Z Planes: " << _z_near << ", " << _z_far << "\n";
}
