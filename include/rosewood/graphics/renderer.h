#ifndef __ROSEWOOD_GRAPHICS_RENDERER_H__
#define __ROSEWOOD_GRAPHICS_RENDERER_H__

#include <memory>

#include "rosewood/core/entity.h"
#include "rosewood/core/component.h"

namespace rosewood { namespace graphics {

    class Mesh;
    class Material;
    class Renderer;
    
    Renderer *renderer(core::Entity entity);

    class Renderer : public core::Component<Renderer> {
    public:
        explicit Renderer(core::Entity owner);

        std::shared_ptr<Mesh> mesh();
        void set_mesh(std::shared_ptr<Mesh> mesh);

        std::shared_ptr<Material> material();
        void set_material(std::shared_ptr<Material> material);
        
    private:
        std::shared_ptr<Mesh> _mesh;
        std::shared_ptr<Material> _material;
    };
    
    inline Renderer *renderer(core::Entity entity) {
        return entity.component<Renderer>();
    }

    inline Renderer::Renderer(core::Entity owner) : core::Component<Renderer>(owner) { }

    inline std::shared_ptr<Mesh> Renderer::mesh() { return _mesh; }
    inline void Renderer::set_mesh(std::shared_ptr<Mesh> mesh) { _mesh = mesh; }

    inline std::shared_ptr<Material> Renderer::material() { return _material; }
    inline void Renderer::set_material(std::shared_ptr<Material> material) { _material = material; }

} }

#endif
