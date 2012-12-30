#ifndef __ROSEWOOD_GRAPHICS_MATERIAL_H__
#define __ROSEWOOD_GRAPHICS_MATERIAL_H__

#include <memory>
#include <vector>

#include "rosewood/graphics/platform_gl.h"

namespace rosewood { namespace math {
    class Matrix4;
} }

namespace rosewood { namespace graphics {

    class Shader;
    class Mesh;
    class Texture;

    class Material {
    public:
        Material();
        ~Material();

        std::shared_ptr<Shader> shader() const;
        void set_shader(std::shared_ptr<Shader> shader);
        
        std::shared_ptr<Texture> texture() const;
        void set_texture(std::shared_ptr<Texture> texture);

        void clear_vertex_buffer();
        void enqueue_mesh(const Mesh *mesh,
                          math::Matrix4 transform, math::Matrix4 inverse_transform);
        bool has_enqueued_meshes() const;
        void submit_draw_calls();
        
        void print_debug_info(std::ostream &os, int indent) const;

    private:
        std::shared_ptr<Shader> _shader;
        std::shared_ptr<Texture> _texture;

        std::vector<float> _vertices;
        GLuint _vbo;
        GLuint _vao;
        size_t _last_size;
        size_t _vertex_index;

        void init_vbo();
        void init_vao();
    };
    
    inline std::shared_ptr<Texture> Material::texture() const { return _texture; }
    inline void Material::set_texture(std::shared_ptr<Texture> texture) { _texture = texture; }

    inline std::shared_ptr<Shader> Material::shader() const { return _shader; }
    inline void Material::set_shader(std::shared_ptr<Shader> shader) { _shader = shader; }

    inline bool Material::has_enqueued_meshes() const { return !!_vertex_index; }

} }

#endif
