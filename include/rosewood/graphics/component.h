#ifndef __ROSEWOOD_GRAPHICS_COMPONENT_H__
#define __ROSEWOOD_GRAPHICS_COMPONENT_H__

#define ROSEWOOD_COMPONENT static ::rosewood::graphics::ComponentTypeCode *static_type_code() \
    { static ::rosewood::graphics::ComponentTypeCode tc; return &tc; } \
    virtual ::rosewood::graphics::ComponentTypeCode *type_code() const override \
    { return static_type_code(); }

#include <ostream>

namespace rosewood { namespace graphics {
    
    struct ComponentTypeCode { };
    
    class SceneObject;
    
    class Component {
    public:
        explicit Component(SceneObject *owner);
        
        virtual ~Component();
        virtual void forward_update();
        virtual void backward_update();
        
        virtual void print_debug_info(std::ostream &os, int indent) const;
        
        virtual ComponentTypeCode *type_code() const = 0;
        
        const SceneObject *owner() const { return _owner; }
        SceneObject *owner() { return _owner; }
        
        Component(const Component&) = delete;
        Component &operator=(const Component&) = delete;
        
    private:
        SceneObject *_owner;
    };
    
} }

#endif
