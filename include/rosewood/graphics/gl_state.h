#ifndef __ROSEWOOD_GRAPHICS_GL_STATE_H__
#define __ROSEWOOD_GRAPHICS_GL_STATE_H__

#include <unordered_map>

#include <stddef.h>

#include "rosewood/data-structures/variant.h"

#include "rosewood/math/math_types.h"

#include "rosewood/graphics/platform_gl.h"

namespace rosewood { namespace graphics {

    typedef rosewood::data_structures::Variant<
        int, math::Matrix3, math::Matrix4, math::Vector4> UniformData;

    static_assert(std::is_same<UniformData::alignment_type, math::Vector4>::value, "incorrect alignment");

    enum VertexAttribPointerType {
        kTypeFloat = GL_FLOAT,
    };

    class VertexAttribPointer {
    public:
        VertexAttribPointer();
        VertexAttribPointer(int size, VertexAttribPointerType type,
                            bool normalized, size_t stride, size_t offset);

        void enable(GLuint attribute) const;

        int size() const;
        VertexAttribPointerType type() const;
        bool normalized() const;
        size_t stride() const;
        size_t offset() const;

    private:
        int _size;
        VertexAttribPointerType _type;
        bool _normalized;
        size_t _stride;
        size_t _offset;
    };

    bool operator==(const VertexAttribPointer &lhs, const VertexAttribPointer &rhs);
    bool operator!=(const VertexAttribPointer &lhs, const VertexAttribPointer &rhs);

    namespace gl_state {

        typedef std::unordered_map<GLint, UniformData> uniform_map;

        void set_state(GLenum state, bool enabled);
        void enable(GLenum state);
        void disable(GLenum state);

        void set_depth_func(GLenum depth_func);
        void set_depth_mask(bool flag);
        void set_blend_func(GLenum sfactor, GLenum dfactor);
        void set_polygon_offset(float factor, float units);

        void bind_vertex_array_object(GLuint vao);
        void bind_array_buffer(GLuint buffer);

        void bind_texture(GLuint texture);
        void delete_texture(GLuint texture);

        void activate_texture_unit(GLuint unit);

        void use_program(GLuint program);

        void set_uniform(GLuint program, GLint uniform, math::Matrix4 m);
        void set_uniform(GLint uniform, math::Matrix4 m);
        void set_uniform(GLuint program, GLint uniform, math::Matrix3 m);
        void set_uniform(GLint uniform, math::Matrix3 m);
        void set_uniform(GLuint program, GLint uniform, math::Vector4 m);
        void set_uniform(GLint uniform, math::Vector4 m);
        void set_uniform(GLuint program, GLint uniform, int i);
        void set_uniform(GLint uniform, int i);

        uniform_map known_uniform_state();
        uniform_map known_uniform_state(GLuint program);

        void set_uniforms(const uniform_map &map);
        void set_uniforms(GLuint program, const uniform_map &map);

        void delete_program(GLuint program);

        void enable_vertex_attrib_array(GLuint attribute);

        void set_vertex_attrib_pointer(GLuint attribute,
                                       VertexAttribPointer pointer);

    }

} }

#endif
