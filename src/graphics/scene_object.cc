#include "rosewood/graphics/scene_object.h"

#include <ostream>

#include "rosewood/math/matrix4.h"
#include "rosewood/math/quaternion.h"
#include "rosewood/math/vector.h"

#include "rosewood/graphics/component.h"

using rosewood::math::Matrix4;
using rosewood::math::Vector3;
using rosewood::math::Quaternion;
using rosewood::math::quaternion_from_axis_angle;

using rosewood::graphics::SceneObject;

SceneObject::SceneObject(std::string name)
    : _name(name), _parent(nullptr), _position(0, 0, 0), _scale(1, 1, 1) {
}

void SceneObject::recursive_update_forward() {
    for (auto &child : _children) {
        child->recursive_update_forward();
    }
    
    for (auto &component : _components) {
        component.second->forward_update();
    }
}

void SceneObject::recursive_update_backward() {
    for (auto &component : _components) {
        component.second->backward_update();
    }
    
    for (auto &child : _children) {
        child->recursive_update_backward();
    }
}

void SceneObject::set_local_axis_angle(float x, float y, float z, float angle) {
    set_local_rotation(quaternion_from_axis_angle(Vector3(x, y, z), angle));
}

Vector3 SceneObject::world_position() const {
    if (!parent()) return local_position();
    return world_to_local(local_position());
}

Quaternion SceneObject::world_rotation() const {
    if (!parent()) return local_rotation();
    return world_to_local(local_rotation());
}

Vector3 SceneObject::world_to_local(Vector3 v) const {
    if (!parent()) return v;
    return parent()->inverse_local_transform() * v;
}

Quaternion SceneObject::world_to_local(math::Quaternion q) const {
    if (!parent()) return q;
    return q * parent()->inverse_world_rotation();
}

Matrix4 SceneObject::local_transform() const {
    Matrix4 t = math::make_translation(_position);
    math::apply_rotate_by(t, _rotation);
    math::apply_scale_by(t, _scale);
    return t;
}

Matrix4 SceneObject::inverse_local_transform() const {
    Matrix4 t = math::make_scale(1/_scale);
    math::apply_rotate_by(t, conjugate(_rotation));
    math::apply_translate_by(t, -_position);
    return t;
}

Matrix4 SceneObject::world_transform() const {
    Matrix4 parent_transform = _parent ? _parent->world_transform() : math::make_identity();
    return parent_transform * local_transform();
}

Matrix4 SceneObject::inverse_world_transform() const {
    Matrix4 inv_transform = inverse_local_transform();
    return _parent ? (inv_transform * _parent->inverse_world_transform()) : inv_transform;
}

Quaternion SceneObject::inverse_world_rotation() const {
    Quaternion q = conjugate(local_rotation());

    return _parent ? (q * _parent->inverse_world_rotation()) : q;
}

void SceneObject::print_debug_info(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "* Scene Object '" << name() << "' " << this << ":\n";
    os << std::string(indent, ' ') << "  Position: (" << _position.x << ", " << _position.y << ", " << _position.z << ")\n";
    os << std::string(indent, ' ') << "  Rotation: (" << _rotation._w << ", " << _rotation._x << ", " << _rotation._y << ", " << _rotation._z << ")\n";
    
    for (auto &component : _components) {
        component.second->print_debug_info(os, indent);
    }

    for (auto &child : _children) {
        child->print_debug_info(os, indent + 2);
    }
}
