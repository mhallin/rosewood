#ifndef __ROSEWOOD_GRAPHICS_RENDERER_H__
#define __ROSEWOOD_GRAPHICS_RENDERER_H__

#include <memory>

#include "component.h"

namespace rosewood { namespace graphics {

    class Mesh;
    class Material;

    class Renderer : public Component {
    public:
        ROSEWOOD_COMPONENT;

        explicit Renderer(SceneObject *owner);

        virtual void print_debug_info(std::ostream &os, int indent) const override;
        
        std::shared_ptr<Mesh> mesh();
        void set_mesh(std::shared_ptr<Mesh> mesh);

        std::shared_ptr<Material> material();
        void set_material(std::shared_ptr<Material> material);
        
    private:
        std::shared_ptr<Mesh> _mesh;
        std::shared_ptr<Material> _material;
    };

    inline Renderer::Renderer(SceneObject *owner) : Component(owner) { }

    inline std::shared_ptr<Mesh> Renderer::mesh() { return _mesh; }
    inline void Renderer::set_mesh(std::shared_ptr<Mesh> mesh) { _mesh = mesh; }

    inline std::shared_ptr<Material> Renderer::material() { return _material; }
    inline void Renderer::set_material(std::shared_ptr<Material> material) { _material = material; }

} }

#endif
