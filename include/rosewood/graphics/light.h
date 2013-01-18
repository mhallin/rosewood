#ifndef __ROSEWOOD_GRAPHICS_LIGHT_H__
#define __ROSEWOOD_GRAPHICS_LIGHT_H__

#include "rosewood/core/component.h"

#include "rosewood/math/math_types.h"

namespace rosewood { namespace graphics {
    
    class Light;
    
    Light *light(core::Entity entity);
    
    class Light : public core::Component<Light> {
    public:
        Light(core::Entity entity) : core::Component<Light>(entity) { }
        
        void set_color(math::Vector4 color) { _color = color; }
        math::Vector4 color() const { return _color; }
            
    private:
        math::Vector4 _color;
    };
    
    inline Light *light(core::Entity entity) {
        return entity.component<Light>();
    }

    
} }

#endif
