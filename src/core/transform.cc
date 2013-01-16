#include "rosewood/core/transform.h"

#include "rosewood/math/matrix4.h"
#include "rosewood/math/quaternion.h"
#include "rosewood/math/vector.h"

using rosewood::core::Entity;
using rosewood::core::Transform;

using rosewood::math::Matrix4;
using rosewood::math::Vector3;
using rosewood::math::Quaternion;
using rosewood::math::quaternion_from_axis_angle;
using rosewood::math::quaternion_identity;

Transform::Transform(Entity entity) : core::Component<Transform>(entity)
, _parent(nullptr), _position(0, 0, 0), _scale(1, 1, 1), _rotation(quaternion_identity()) {
}

void Transform::set_local_axis_angle(float x, float y, float z, float angle) {
    set_local_rotation(quaternion_from_axis_angle(Vector3(x, y, z), angle));
}

Vector3 Transform::world_position() const {
    if (!parent()) return local_position();
    return world_to_local(local_position());
}

Quaternion Transform::world_rotation() const {
    if (!parent()) return local_rotation();
    return world_to_local(local_rotation());
}

Vector3 Transform::world_to_local(Vector3 v) const {
    if (!parent()) return v;
    return parent()->inverse_local_transform() * v;
}

Quaternion Transform::world_to_local(math::Quaternion q) const {
    if (!parent()) return q;
    return q * parent()->inverse_world_rotation();
}

Matrix4 Transform::local_transform() const {
    Matrix4 t = math::make_translation(_position);
    math::apply_rotate_by(t, _rotation);
    math::apply_scale_by(t, _scale);
    return t;
}

Matrix4 Transform::inverse_local_transform() const {
    Matrix4 t = math::make_scale(1/_scale);
    math::apply_rotate_by(t, conjugate(_rotation));
    math::apply_translate_by(t, -_position);
    return t;
}

Matrix4 Transform::world_transform() const {
    Matrix4 parent_transform = _parent ? _parent->world_transform() : math::make_identity();
    return parent_transform * local_transform();
}

Matrix4 Transform::inverse_world_transform() const {
    Matrix4 inv_transform = inverse_local_transform();
    return _parent ? (inv_transform * _parent->inverse_world_transform()) : inv_transform;
}

Quaternion Transform::inverse_world_rotation() const {
    Quaternion q = conjugate(local_rotation());
    
    return _parent ? (q * _parent->inverse_world_rotation()) : q;
}
