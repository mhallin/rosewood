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

Transform::Transform(Entity entity)
: core::Component<Transform>(entity)
, _parent(nullptr), _local_position(0, 0, 0)
, _local_scale(1, 1, 1), _local_rotation(quaternion_identity())
, _transform_matrices_invalid(true) {
}

void Transform::set_local_axis_angle(float x, float y, float z, float angle) {
    set_local_rotation(quaternion_from_axis_angle(Vector3(x, y, z), angle));
}

void Transform::set_world_axis_angle(float x, float y, float z, float angle) {
    set_world_rotation(quaternion_from_axis_angle(Vector3(x, y, z), angle));
}

Quaternion Transform::world_rotation() const {
    Quaternion q = local_rotation();
    return parent() ? (parent()->world_rotation() * q) : q;
}

Quaternion Transform::inverse_world_rotation() const {
    Quaternion q = conjugate(local_rotation());
    return _parent ? (q * _parent->inverse_world_rotation()) : q;
}

Vector3 Transform::world_to_local(Vector3 v) const {
    return inverse_world_transform() * v;
}

Quaternion Transform::world_to_local(Quaternion q) const {
    return q * inverse_world_rotation();
}

Vector3 Transform::local_to_world(Vector3 v) const {
    return world_transform() * v;
}

Quaternion Transform::local_to_world(Quaternion q) const {
    return world_rotation() * q;
}

void Transform::set_local_position_preserving_child_world_positions(Vector3 v) {
    std::vector<Vector3> child_positions;
    for (auto child : _children) {
        child_positions.push_back(child->world_position());
    }
    _local_position = v;
    invalidate_transform_matrices();
    for (size_t i = 0; i < _children.size(); ++i) {
        _children[i]->set_world_position(child_positions[i]);
    }
}

void Transform::construct_transform_matrices() const {
    Matrix4 local_transform = math::make_translation(_local_position);
    math::apply_rotate_by(local_transform, _local_rotation);
    math::apply_scale_by(local_transform, _local_scale);
    
    Matrix4 inverse_local_transform = math::make_scale(1.0f / _local_scale);
    math::apply_rotate_by(inverse_local_transform, conjugate(_local_rotation));
    math::apply_translate_by(inverse_local_transform, -_local_position);
    
    Matrix4 parent_world_transform = _parent ? _parent->world_transform() : math::make_identity();
    Matrix4 inverse_parent_world_transform = (_parent
                                              ? _parent->inverse_world_transform()
                                              : math::make_identity());
    
    _local_transform = local_transform;
    _inverse_local_transform = inverse_local_transform;
    _world_transform = parent_world_transform * _local_transform;
    _inverse_world_transform = _inverse_local_transform * inverse_parent_world_transform;
    _transform_matrices_invalid = false;
}
