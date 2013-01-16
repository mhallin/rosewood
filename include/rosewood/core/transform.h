#ifndef __ROSEWOOD_CORE_TRANSFORM_H__
#define __ROSEWOOD_CORE_TRANSFORM_H__

#include "rosewood/math/math_types.h"

#include "rosewood/core/component.h"

namespace rosewood { namespace core {
    
    class Transform;
    
    Transform *transform(Entity entity);
    
    class Transform : public Component<Transform> {
    public:
        Transform(Entity owner);

        const Transform *parent() const { return _parent; }
        Transform *parent() { return _parent; }
        
        void add_child(Transform *child);
        const std::vector<Transform*> &children() const { return _children; }
        
        math::Vector3 local_position() const { return _local_position; }
        math::Quaternion local_rotation() const { return _local_rotation; }
        math::Vector3 local_scale() const { return _local_scale; }
        
        void set_local_position(math::Vector3 v);
        void set_local_position(float x, float y, float z);
        void set_local_rotation(math::Quaternion rotation);
        void set_local_axis_angle(float x, float y, float z, float angle);
        void set_local_scale(math::Vector3 v);
        void set_local_scale(float x, float y, float z);
        
        math::Vector3 world_position() const;
        math::Quaternion world_rotation() const;
        
        void set_world_position(math::Vector3 v);
        void set_world_position(float x, float y, float z);
        
        math::Vector3 world_to_local(math::Vector3 v) const;
        math::Quaternion world_to_local(math::Quaternion q) const;
        
        math::Matrix4 local_transform() const;
        math::Matrix4 inverse_local_transform() const;
        
        math::Matrix4 world_transform() const;
        math::Matrix4 inverse_world_transform() const;
        
        math::Quaternion inverse_world_rotation() const;

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
        
        void invalidate_transform_matrices();
        void construct_transform_matrices_if_invalid() const;
        void construct_transform_matrices() const;
    };
    
    inline Transform *transform(Entity entity) {
        return entity.component<Transform>();
    }

    inline void Transform::add_child(Transform *child) {
        _children.push_back(child);
        child->_parent = this;
        child->invalidate_transform_matrices();
    }
    
    inline void Transform::set_local_position(math::Vector3 v) {
        _local_position = v;
        invalidate_transform_matrices();
    }
    
    inline void Transform::set_local_position(float x, float y, float z) {
        set_local_position(math::Vector3(x, y, z));
    }
    
    inline void Transform::set_local_rotation(math::Quaternion rotation) {
        _local_rotation = rotation;
        invalidate_transform_matrices();
    }
    
    inline void Transform::set_local_scale(math::Vector3 v) {
        _local_scale = v;
        invalidate_transform_matrices();
    }
    
    inline void Transform::set_local_scale(float x, float y, float z) {
        set_local_scale(math::Vector3(x, y, z));
    }
    
    inline void Transform::set_world_position(math::Vector3 v) {
        set_local_position(world_to_local(v));
    }
    
    inline void Transform::set_world_position(float x, float y, float z) {
        set_world_position(math::Vector3(x, y, z));
    }
    
    inline math::Matrix4 Transform::local_transform() const {
        construct_transform_matrices_if_invalid();
        return _local_transform;
    }
    
    inline math::Matrix4 Transform::inverse_local_transform() const {
        construct_transform_matrices_if_invalid();
        return _inverse_local_transform;
    }
    
    inline math::Matrix4 Transform::world_transform() const {
        construct_transform_matrices_if_invalid();
        return _world_transform;
    }
    
    inline math::Matrix4 Transform::inverse_world_transform() const {
        construct_transform_matrices_if_invalid();
        return _inverse_world_transform;
    }
    
    inline void Transform::invalidate_transform_matrices() {
        _transform_matrices_invalid = true;
        for (auto child : _children) {
            child->invalidate_transform_matrices();
        }
    }
    
    inline void Transform::construct_transform_matrices_if_invalid() const {
        if (_transform_matrices_invalid) {
            construct_transform_matrices();
        }
    }

} }

#endif
