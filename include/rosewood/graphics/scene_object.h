#ifndef __ROSEWOOD_GRAPHICS_SCENE_OBJECT_H__
#define __ROSEWOOD_GRAPHICS_SCENE_OBJECT_H__

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "rosewood/math/math_types.h"

namespace rosewood { namespace math {
    class Matrix4;
} }

namespace rosewood { namespace graphics {
    
    struct ComponentTypeCode;
    class Component;
    
    class SceneObject {
    public:
        explicit SceneObject(std::string name);
        
        void recursive_update_forward();
        void recursive_update_backward();
        
        const std::string &name() const;
        void set_name(std::string name);

        const SceneObject *parent() const;
        SceneObject *parent();
        
        void add_child(std::shared_ptr<SceneObject> child);
        const std::vector<std::shared_ptr<SceneObject>> &children() const;
        
        template<typename TComponent> TComponent *component();
        template<typename TComponent> const TComponent *component() const;
        template<typename TComponent, typename... TArgs> TComponent *add_component(TArgs... args);
        template<typename TComponent> void remove_component();
        
        math::Vector3 local_position() const;
        math::Quaternion local_rotation() const;
        math::Vector3 local_scale() const;
        
        void set_local_position(math::Vector3 v);
        void set_local_position(float x, float y, float z);
        void set_local_rotation(math::Quaternion rotation);
        void set_local_axis_angle(float x, float y, float z, float angle);
        void set_local_scale(float x, float y, float z);
        
        math::Vector3 world_position() const;
        math::Quaternion world_rotation() const;
        
        math::Vector3 world_to_local(math::Vector3 v) const;
        math::Quaternion world_to_local(math::Quaternion q) const;
        
        math::Matrix4 local_transform() const;
        math::Matrix4 inverse_local_transform() const;
        
        math::Matrix4 world_transform() const;
        math::Matrix4 inverse_world_transform() const;
        
        math::Quaternion inverse_world_rotation() const;
        
        void print_debug_info(std::ostream &os, int indent = 0) const;
        
    private:
        std::string _name;
        std::unordered_map<ComponentTypeCode*, std::unique_ptr<Component>> _components;
        std::vector<std::shared_ptr<SceneObject>> _children;
        SceneObject *_parent;
        
        math::Vector3 _position;
        math::Quaternion _rotation;
        math::Vector3 _scale;
    };
    
    inline const std::string &SceneObject::name() const { return _name; }

    inline const SceneObject *SceneObject::parent() const { return _parent; }
    inline SceneObject *SceneObject::parent() { return _parent; }

    inline void SceneObject::add_child(std::shared_ptr<SceneObject> child) {
        _children.push_back(child);
        child->_parent = this;
    }

    inline const std::vector<std::shared_ptr<SceneObject>> &SceneObject::children() const {
        return _children;
    }
    
    template<typename TComponent>
    inline TComponent *SceneObject::component() {
        auto it = _components.find(TComponent::static_type_code());
        if (it == _components.end()) {
            return nullptr;
        }
        return static_cast<TComponent*>(it->second.get());
    }
    
    template<typename TComponent>
    inline const TComponent *SceneObject::component() const {
        return const_cast<SceneObject*>(this)->component<TComponent>();
    }
    
    template<typename TComponent, typename... TArgs>
    inline TComponent *SceneObject::add_component(TArgs... args) {
        auto comp = new TComponent(this, std::forward<TArgs>(args)...);
        _components[TComponent::static_type_code()] = std::unique_ptr<Component>(comp);

        return comp;
    }
    
    inline math::Vector3 SceneObject::local_position() const { return _position; }
    inline math::Quaternion SceneObject::local_rotation() const { return _rotation; }
    inline math::Vector3 SceneObject::local_scale() const { return _scale; }
    
    inline void SceneObject::set_local_position(math::Vector3 v) {
        _position = v;
    }
    
    inline void SceneObject::set_local_position(float x, float y, float z) {
        _position = math::Vector3(x, y, z);
    }

    inline void SceneObject::set_local_rotation(math::Quaternion rotation) {
        _rotation = rotation;
    }
    
    inline void SceneObject::set_local_scale(float x, float y, float z) {
        _scale = math::Vector3(x, y, z);
    }
    
} }

#endif
