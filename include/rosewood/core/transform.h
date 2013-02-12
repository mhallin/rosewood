#ifndef __ROSEWOOD_CORE_TRANSFORM_H__
#define __ROSEWOOD_CORE_TRANSFORM_H__

#include "rosewood/math/math_types.h"

#include "rosewood/core/component.h"

namespace rosewood { namespace core {

    enum class ChildPositions {
        KeepLocal,
        KeepWorld,
    };

    class Transform;

    Transform *transform(Entity entity);

    class Transform : public Component<Transform> {
    public:
        explicit Transform(Entity owner);

        const Transform *parent() const { return _parent; }
        Transform *parent() { return _parent; }

        void add_child(Transform *child);
        const std::vector<Transform*> &children() const { return _children; }

        void remove_from_parent();


        // Getting/setting local transform
        math::Vector3 local_position() const { return _local_position; }
        math::Quaternion local_rotation() const { return _local_rotation; }
        math::Vector3 local_scale() const { return _local_scale; }


        void set_local_position(math::Vector3 v, ChildPositions cpos = ChildPositions::KeepLocal) {
            if (cpos == ChildPositions::KeepLocal) {
                _local_position = v;
                invalidate_transform_matrices();
            }
            else {
                set_local_position_preserving_child_world_positions(v);
            }
        }

        void set_local_position(float x, float y, float z, ChildPositions cpos = ChildPositions::KeepLocal) {
            set_local_position(math::Vector3(x, y, z), cpos);
        }

        void set_local_rotation(math::Quaternion rotation) {
            _local_rotation = rotation; invalidate_transform_matrices();
        }

        void set_local_axis_angle(float x, float y, float z, float angle);

        void set_local_scale(math::Vector3 v) {
            _local_scale = v; invalidate_transform_matrices();
        }

        void set_local_scale(float x, float y, float z) {
            set_local_scale(math::Vector3(x, y, z));
        }


        // Getting/setting world transform
        math::Vector3 world_position() const { return local_to_world(math::Vector3(0, 0, 0)); }
        math::Quaternion world_rotation() const;
        math::Quaternion inverse_world_rotation() const;


        void set_world_position(math::Vector3 v, ChildPositions cpos = ChildPositions::KeepLocal) {
            if (_parent) {
                set_local_position(_parent->world_to_local(v), cpos);
            }
            else {
                set_local_position(v, cpos);
            }
        }

        void set_world_position(float x, float y, float z, ChildPositions cpos = ChildPositions::KeepLocal) {
            set_world_position(math::Vector3(x, y, z), cpos);
        }

        void set_world_rotation(math::Quaternion rotation) {
            set_local_rotation(world_to_local(rotation));
        }

        void set_world_axis_angle(float x, float y, float z, float angle);

        // Converting positions/rotations
        math::Vector3 world_to_local(math::Vector3 v) const;
        math::Quaternion world_to_local(math::Quaternion q) const;

        math::Vector3 local_to_world(math::Vector3 v) const;
        math::Quaternion local_to_world(math::Quaternion q) const;

        math::Vector3 convert_from(math::Vector3 v, Transform *from_transform) const {
            return world_to_local(from_transform->local_to_world(v));
        }

        math::Quaternion convert_from(math::Quaternion q, Transform *from_transform) const {
            return world_to_local(from_transform->local_to_world(q));
        }


        math::Vector3 convert_to(math::Vector3 v, Transform *to_transform) const {
            return to_transform->world_to_local(local_to_world(v));
        }

        math::Quaternion convert_to(math::Quaternion q, Transform *to_transform) const {
            return to_transform->world_to_local(local_to_world(q));
        }


        // Extracting transform matrices
        math::Matrix4 local_transform() const {
            construct_transform_matrices_if_invalid();
            return _local_transform;
        }

        math::Matrix4 inverse_local_transform() const {
            construct_transform_matrices_if_invalid();
            return _inverse_local_transform;
        }


        math::Matrix4 world_transform() const {
            construct_transform_matrices_if_invalid();
            return _world_transform;
        }

        math::Matrix4 inverse_world_transform() const {
            construct_transform_matrices_if_invalid();
            return _inverse_world_transform;
        }

    private:
        std::vector<Transform*> _children;
        Transform *_parent;

        math::Vector3 _local_position;
        math::Quaternion _local_rotation;
        math::Vector3 _local_scale;

        mutable math::Matrix4 _local_transform;
        mutable math::Matrix4 _inverse_local_transform;

        mutable math::Matrix4 _world_transform;
        mutable math::Matrix4 _inverse_world_transform;

        mutable bool _transform_matrices_invalid;

        void set_local_position_preserving_child_world_positions(math::Vector3 v);

        void invalidate_transform_matrices();
        void construct_transform_matrices() const;

        void construct_transform_matrices_if_invalid() const {
            if (_transform_matrices_invalid) {
                construct_transform_matrices();
            }
        }
    };

    inline Transform *transform(Entity entity) {
        return entity.component<Transform>();
    }

    inline void Transform::add_child(Transform *child) {
        if (child->_parent) {
            child->remove_from_parent();
        }

        _children.push_back(child);
        child->_parent = this;
        child->invalidate_transform_matrices();
    }

    inline void Transform::remove_from_parent() {
        auto it = std::find(begin(_parent->children()), end(_parent->children()), this);
        _parent->_children.erase(it);
        _parent = nullptr;
        invalidate_transform_matrices();
    }

    inline void Transform::invalidate_transform_matrices() {
        _transform_matrices_invalid = true;
        for (auto child : _children) {
            child->invalidate_transform_matrices();
        }
    }

} }

#endif
