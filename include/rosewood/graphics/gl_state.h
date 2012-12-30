#ifndef __ROSEWOOD_GRAPHICS_GL_STATE_H__
#define __ROSEWOOD_GRAPHICS_GL_STATE_H__

#include <cstddef>

#include "rosewood/math/math_types.h"

#include "rosewood/graphics/platform_gl.h"

namespace rosewood { namespace graphics {

    enum class UniformDataType {
        Empty,
        Int1,
        Matrix3, Matrix4,
    };

    union UniformDataUnion {
        int i1;
        math::Matrix3 mat3;
        math::Matrix4 mat4;

        explicit UniformDataUnion(int i1) : i1(i1) { }
        explicit UniformDataUnion(math::Matrix3 mat3) : mat3(mat3) { }
        explicit UniformDataUnion(math::Matrix4 mat4) : mat4(mat4) { }
    };

    struct UniformData {
        UniformDataType type;
        UniformDataUnion data;

        UniformData();
        explicit UniformData(int i1);
        explicit UniformData(math::Matrix3 mat3);
        explicit UniformData(math::Matrix4 mat4);
    };

    bool operator== (const UniformData &lhs, const UniformData &rhs);
    bool operator!= (const UniformData &lhs, const UniformData &rhs);

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

        void set_state(GLenum state, bool enabled);
        void enable(GLenum state);
        void disable(GLenum state);

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
        void set_uniform(GLuint program, GLint uniform, int i);
        void set_uniform(GLint uniform, int i);

        void delete_program(GLuint program);

        void enable_vertex_attrib_array(GLuint attribute);

        void set_vertex_attrib_pointer(GLuint attribute,
                                       VertexAttribPointer pointer);

    }

} }

#endif
