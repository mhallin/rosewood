#include "rosewood/graphics/gl_state.h"

#include <assert.h>

#include <iostream>
#include <functional>
#include <unordered_map>

#include "rosewood/core/stats.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/matrix3.h"
#include "rosewood/math/vector.h"

#include "rosewood/graphics/gl_func.h"

using rosewood::math::Matrix3;
using rosewood::math::Matrix4;
using rosewood::math::Vector4;

namespace rosewood { namespace graphics { namespace gl_state {

    // Helper function for state management: Looks up `key` in `map`,
    // compares it to `value` and calls `if_changed_fn` if they aren't
    // equal (or if `key` does not exist). Updates `map` after the
    // callback has been called
    template<typename TKey, typename TValue, typename TNewValue>
    static void if_changed(std::unordered_map<TKey, TValue> &map,
                           TKey key, const TNewValue &value,
                           std::function<void()> if_changed_fn) {
        if (map.find(key) == end(map) || map.at(key) != value) {
            if_changed_fn();
            map[key] = value;
        }
    }

    // Helper function for state management: Calls `if_changed_fn` if
    // `known_value` differs from `new_value` and updates `known_value`
    // afterwards.
    template<typename TValue>
    static void if_changed(TValue &known_value, const TValue new_value,
                           std::function<void()> if_changed_fn) {
        if (known_value != new_value) {
            if_changed_fn();
            known_value = new_value;
        }
    }

    static std::unordered_map<GLenum, bool> gKnownState; // glEnable/glDisable
    static GLuint gCurrentBuffer = UINT_MAX; // glBindBuffer
    static GLuint gCurrentVAO = UINT_MAX; // glBindVertexArray
    static GLuint gCurrentTexture = UINT_MAX; // glBindTexture
    static GLuint gCurrentProgram = UINT_MAX; // glUseProgram
    static GLuint gCurrentTextureUnit = UINT_MAX; // glActiveTexture
    static GLenum gCurrentDepthFunc = GL_LESS;
    static bool gCurrentDepthMask = true;
    static std::pair<GLenum, GLenum> gCurrentBlendFunc = std::make_pair(GL_ONE, GL_ZERO);
    static std::pair<float, float> gCurrentPolygonOffset = std::make_pair(0, 0);
    static uniform_map gCurrentUniforms; // All glUniform*
    static std::unordered_map<GLuint, uniform_map> gSavedShaderStates;
    static std::unordered_map<GLuint, uniform_map> gNewShaderStates;
    static std::unordered_map<GLuint, bool> gKnownAttribArrays; // glEnableVertexAttribArray
    static std::unordered_map<GLuint, VertexAttribPointer> gCurrentVertexAttribPointers; // glVertexAttribPointer

    void set_state(GLenum state, bool enabled) {
        if_changed(gKnownState, state, enabled, [=]() {
            if (enabled) {
                GL_FUNC(glEnable)(state);
            } else {
                GL_FUNC(glDisable)(state);
            }
        });
    }

    void enable (GLenum state) { set_state(state, true); }
    void disable(GLenum state) { set_state(state, false); }

    void set_depth_func(GLenum depth_func) {
        if_changed(gCurrentDepthFunc, depth_func, [=]() {
            GL_FUNC(glDepthFunc)(depth_func);
        });
    }

    void set_depth_mask(bool depth_mask) {
        if_changed(gCurrentDepthMask, depth_mask, [=]() {
            GL_FUNC(glDepthMask)(depth_mask ? GL_TRUE : GL_FALSE);
        });
    }

    void set_blend_func(GLenum sfactor, GLenum dfactor) {
        auto blend_func = std::make_pair(sfactor, dfactor);
        if_changed(gCurrentBlendFunc, blend_func, [=]() {
            GL_FUNC(glBlendFunc)(sfactor, dfactor);
        });
    }

    void set_polygon_offset(float factor, float units) {
        auto polygon_offset = std::make_pair(factor, units);
        if_changed(gCurrentPolygonOffset, polygon_offset, [=]() {
            GL_FUNC(glPolygonOffset)(factor, units);
        });
    }

    void bind_vertex_array_object(GLuint vao) {
        if_changed(gCurrentVAO, vao, [=]() {
            GL_FUNC(glBindVertexArray)(vao);
            gCurrentBuffer = UINT_MAX;
            gCurrentVertexAttribPointers.clear();
            gKnownAttribArrays.clear();
        });
    }

    void bind_array_buffer(GLuint buffer) {
        if_changed(gCurrentBuffer, buffer, [=]() {
            GL_FUNC(glBindBuffer)(GL_ARRAY_BUFFER, buffer);
            gCurrentVertexAttribPointers.clear();
        });
    }

    void bind_texture(GLuint texture) {
        if_changed(gCurrentTexture, texture, [=]() {
            GL_FUNC(glBindTexture)(GL_TEXTURE_2D, texture);
        });
    }

    void delete_texture(GLuint texture) {
        if (texture == gCurrentTexture) {
            bind_texture(0);
        }

        GL_FUNC(glDeleteTextures)(1, &texture);
    }

    void activate_texture_unit(GLuint unit) {
        if_changed(gCurrentTextureUnit, unit, [=] {
            GL_FUNC(glActiveTexture)(GL_TEXTURE0 + unit);
        });
    }

    void use_program(GLuint program) {
        if_changed(gCurrentProgram, program, [=]() {
            GL_FUNC(glUseProgram)(program);
            core::stats::shader_change_count.increment();

            gSavedShaderStates[gCurrentProgram] = gCurrentUniforms;
            gCurrentUniforms = gSavedShaderStates[program];
            gCurrentProgram = program;

            set_uniforms(program, gNewShaderStates[program]);

            gNewShaderStates[program].clear();
        });
    }

    uniform_map known_uniform_state() {
        return gCurrentUniforms;
    }

    uniform_map known_uniform_state(GLuint program) {
        if (program == gCurrentProgram) {
            return known_uniform_state();
        }

        uniform_map state = gSavedShaderStates[program];
        for (auto pair : gNewShaderStates[program]) {
            state[pair.first] = pair.second;
        }

        return state;
    }

    void set_uniforms(const uniform_map &map) {
        set_uniforms(gCurrentProgram, map);
    }

    void set_uniforms(GLuint program, const uniform_map &map) {
        for (auto pair : map) {
            if (pair.second.has<int>()) {
                set_uniform(program, pair.first, pair.second.get<int>());
            }
            else if (pair.second.has<Matrix3>()) {
                set_uniform(program, pair.first, pair.second.get<Matrix3>());
            }
            else if (pair.second.has<Matrix4>()) {
                set_uniform(program, pair.first, pair.second.get<Matrix4>());
            }
            else if (pair.second.has<Vector4>()) {
                set_uniform(program, pair.first, pair.second.get<Vector4>());
            }
        }
    }

    void set_uniform(GLuint program, GLint uniform, math::Matrix4 m) {
        if (gCurrentProgram == program) {
            set_uniform(uniform, m);
        }
        else {
            gNewShaderStates[program][uniform] = m;
        }
    }

    void set_uniform(GLint uniform, math::Matrix4 m) {
        if (gCurrentProgram == UINT_MAX) return;

        if_changed(gCurrentUniforms, uniform, m, [&](){
            GL_FUNC(glUniformMatrix4fv)(uniform, 1, GL_FALSE, ptr(m));
        });
    }

    void set_uniform(GLuint program, GLint uniform, math::Matrix3 m) {
        if (gCurrentProgram == program) {
            set_uniform(uniform, m);
        }
        else {
            gNewShaderStates[program][uniform] = m;
        }
    }

    void set_uniform(GLint uniform, math::Matrix3 m) {
        if (gCurrentProgram == UINT_MAX) return;

        if_changed(gCurrentUniforms, uniform, m, [&](){
            GL_FUNC(glUniformMatrix3fv)(uniform, 1, GL_FALSE, ptr(m));
        });
    }

    void set_uniform(GLuint program, GLint uniform, math::Vector4 vec4) {
        if (gCurrentProgram == program) {
            set_uniform(uniform, vec4);
        }
        else {
            gNewShaderStates[program][uniform] = vec4;
        }
    }

    void set_uniform(GLint uniform, math::Vector4 vec4) {
        if (gCurrentProgram == UINT_MAX) return;

        if_changed(gCurrentUniforms, uniform, vec4, [=](){
            GL_FUNC(glUniform4fv)(uniform, 1, math::ptr(vec4));
        });
    }

    void set_uniform(GLuint program, GLint uniform, int i) {
        if (gCurrentProgram == program) {
            set_uniform(uniform, i);
        }
        else {
            gNewShaderStates[program][uniform] = i;
        }
    }

    void set_uniform(GLint uniform, int i) {
        if (gCurrentProgram == UINT_MAX) return;

        if_changed(gCurrentUniforms, uniform, i, [=](){
            GL_FUNC(glUniform1i)(uniform, i);
        });
    }

    void delete_program(GLuint program) {
        gSavedShaderStates[program].clear();
        gNewShaderStates[program].clear();

        if (gCurrentProgram == program) {
            gCurrentUniforms.clear();
            use_program(0);
        }

        GL_FUNC(glDeleteProgram)(program);
    }

    void enable_vertex_attrib_array(GLuint attribute) {
        if_changed(gKnownAttribArrays, attribute, true, [=]() {
            GL_FUNC(glEnableVertexAttribArray)(attribute);
        });
    }

    void set_vertex_attrib_pointer(GLuint attribute, VertexAttribPointer pointer) {
        if_changed(gCurrentVertexAttribPointers, attribute, pointer, [&]() {
            pointer.enable(attribute);
        });
    }

} } }

namespace rosewood { namespace graphics {

    VertexAttribPointer::VertexAttribPointer()
    : _size(-1), _type(kTypeFloat), _normalized(false), _stride(0), _offset(0) { }

    VertexAttribPointer::VertexAttribPointer(int size,
                                             VertexAttribPointerType type,
                                             bool normalized,
                                             size_t stride,
                                             size_t offset)
    : _size(size), _type(type), _normalized(normalized), _stride(stride), _offset(offset) { }

    void VertexAttribPointer::enable(GLuint attribute) const {
        GL_FUNC(glVertexAttribPointer)(attribute,
                                       _size,
                                       (GLenum)_type,
                                       _normalized ? GL_TRUE : GL_FALSE,
                                       (int)_stride,
                                       ((char*)0) + _offset);
    }

    int VertexAttribPointer::size() const { return _size; }
    VertexAttribPointerType VertexAttribPointer::type() const { return _type; }
    bool VertexAttribPointer::normalized() const { return _normalized; }
    size_t VertexAttribPointer::stride() const { return _stride; }
    size_t VertexAttribPointer::offset() const { return _offset; }

    bool operator==(const VertexAttribPointer &lhs, const VertexAttribPointer &rhs) {
        return (lhs.size() == rhs.size()
                && lhs.type() == rhs.type()
                && lhs.normalized() == rhs.normalized()
                && lhs.stride() == rhs.stride()
                && lhs.offset() == rhs.offset());
    }

    bool operator!=(const VertexAttribPointer &lhs, const VertexAttribPointer &rhs) {
        return !(lhs == rhs);
    }

} }
