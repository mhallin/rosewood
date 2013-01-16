#ifndef __ROSEWOOD_CORE_COMPONENT_H__
#define __ROSEWOOD_CORE_COMPONENT_H__

#include "rosewood/core/entity.h"

namespace rosewood { namespace core {
    
    typedef unsigned int ComponentTypeCode;
    
    struct Entity;
    
    class BaseComponent {
    public:
        explicit BaseComponent(Entity entity) : _entity(entity) { }
        
        Entity entity() const { return _entity; }

    protected:
        static ComponentTypeCode _type_code_counter;

    private:
        Entity _entity;
    };
    
    template<class Derived>
    class Component : public BaseComponent {
    public:
        explicit Component(Entity entity) : BaseComponent(entity) { }
        
        static ComponentTypeCode register_type() {
            static auto type_code = _type_code_counter++;
            return type_code;
        }
    };
    
} }

#endif
