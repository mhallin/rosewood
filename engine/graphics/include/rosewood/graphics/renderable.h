#ifndef __ROSEWOOD_GRAPHICS_RENDERER_H__
#define __ROSEWOOD_GRAPHICS_RENDERER_H__

#include <memory>

#include "rosewood/core/entity.h"
#include "rosewood/core/component.h"

namespace rosewood { namespace graphics {

    class Mesh;
    class Material;

    class Renderable : public core::Component<Renderable> {
    public:
        explicit Renderable(core::Entity entity)
        : core::Component<Renderable>(entity), _enabled(true) { }

        std::shared_ptr<Mesh> mesh() { return _mesh; }
        void set_mesh(std::shared_ptr<Mesh> mesh) { _mesh = mesh; }

        std::shared_ptr<Material> material() { return _material; }
        void set_material(std::shared_ptr<Material> material) { _material = material; }

        bool enabled() const { return _enabled; }
        void set_enabled(bool enabled) { _enabled = enabled; }

    private:
        std::shared_ptr<Mesh> _mesh;
        std::shared_ptr<Material> _material;

        bool _enabled;
    };

} }

#endif
