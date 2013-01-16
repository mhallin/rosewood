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

        const Transform *parent() const;
        Transform *parent();
        
        void add_child(Transform *child);
        const std::vector<Transform*> &children() const;
        
        math::Vector3 local_position() const;
        math::Quaternion local_rotation() const;
        math::Vector3 local_scale() const;
        
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
        
        math::Vector3 _position;
        math::Quaternion _rotation;
        math::Vector3 _scale;
    };
    
    inline Transform *transform(Entity entity) {
        return entity.component<Transform>();
    }

    inline const Transform *Transform::parent() const { return _parent; }
    inline Transform *Transform::parent() { return _parent; }
    
    inline void Transform::add_child(Transform *child) {
        _children.push_back(child);
        child->_parent = this;
    }
    
    inline const std::vector<Transform*> &Transform::children() const {
        return _children;
    }
    
    inline math::Vector3 Transform::local_position() const { return _position; }
    inline math::Quaternion Transform::local_rotation() const { return _rotation; }
    inline math::Vector3 Transform::local_scale() const { return _scale; }

    inline void Transform::set_local_position(math::Vector3 v) {
        _position = v;
    }
    
    inline void Transform::set_local_position(float x, float y, float z) {
        _position = math::Vector3(x, y, z);
    }
    
    inline void Transform::set_local_rotation(math::Quaternion rotation) {
        _rotation = rotation;
    }
    
    inline void Transform::set_local_scale(math::Vector3 v) {
        _scale = v;
    }
    
    inline void Transform::set_local_scale(float x, float y, float z) {
        _scale = math::Vector3(x, y, z);
    }
    
    inline void Transform::set_world_position(math::Vector3 v) {
        set_local_position(world_to_local(v));
    }
    
    inline void Transform::set_world_position(float x, float y, float z) {
        set_world_position(math::Vector3(x, y, z));
    }

} }

#endif
