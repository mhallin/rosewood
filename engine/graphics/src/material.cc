#include "rosewood/graphics/material.h"

#include <ostream>

#include "rosewood/core/assert.h"
#include "rosewood/core/stats.h"

#include "rosewood/math/matrix4.h"

#include "rosewood/graphics/gl_state.h"
#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/gl_func.h"
#include "rosewood/graphics/texture.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

using rosewood::math::Matrix4;

using rosewood::graphics::Material;

Material::Material()
: _light(nullptr), _vbo(UINT_MAX), _vao(UINT_MAX)
, _last_size(0), _buffer_index(0), _vertex_count(0) { }

Material::~Material() {
    if (_vbo != UINT_MAX) {
        GL_FUNC(glDeleteBuffers)(1, &_vbo);
    }
    if (_vao != UINT_MAX) {
        GL_FUNC(glDeleteVertexArrays)(1, &_vao);
    }
}

void Material::clear_vertex_buffer() {
    _buffer_index = 0;
    _vertex_count = 0;
}

void Material::enqueue_mesh(const Mesh *mesh, Matrix4 transform, Matrix4 inverse_transform) {
    RW_ASSERT(mesh, "Expected a mesh to enqueue_mesh");
    RW_ASSERT(shader(), "Material must have shader set");

    auto nverts = mesh->vertex_data().size();
    auto meshbufsize = nverts * shader()->attribute_stride() / sizeof(float);

	RW_ASSERT(nverts, "Mesh must have vertex data");

    if (_buffer_index + meshbufsize > _buffer.size()) {
        _buffer.resize(_buffer_index + meshbufsize);
    }
    mesh->instantiate(transform, inverse_transform, _buffer.begin() + _buffer_index,
                      shader()->extra_attributes());
    _buffer_index += meshbufsize;
    _vertex_count += nverts;
}

void Material::submit_draw_calls() {
    if (!_buffer_index) return;
    if (_vbo == UINT_MAX) init_vbo();
    if (_vao == UINT_MAX) init_vao();

    gl_state::bind_vertex_array_object(_vao);
    gl_state::bind_array_buffer(_vbo);

    bind_texture();
    upload_vbo_data();
    draw_triangles();
}

void Material::print_debug_info(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "- Material " << this << "\n";
    os << std::string(indent, ' ') << "  VAO: " << _vao << ", VBO: " << _vbo << ", Buffer size: " << _buffer.size() << "\n";
}

void Material::init_vbo() {
    GL_FUNC(glGenBuffers)(1, &_vbo);
}

void Material::init_vao() {
    GL_FUNC(glGenVertexArrays)(1, &_vao);

    gl_state::bind_vertex_array_object(_vao);
    gl_state::bind_array_buffer(_vbo);

    shader()->initialize_attribute_arrays();
}

void Material::bind_texture() const {
    if (_texture) {
        gl_state::activate_texture_unit(0);
        _texture->bind();
        _shader->set_texture_sampler_uniform(0);
    }
    else {
        gl_state::bind_texture(0);
    }
}

void Material::upload_vbo_data() {
    auto new_size = _buffer_index * sizeof(float);

    if (new_size > _last_size) {
        GL_FUNC(glBufferData)(GL_ARRAY_BUFFER, new_size, &_buffer[0], GL_DYNAMIC_DRAW);
        _last_size = new_size;
    }
    else {
        GL_FUNC(glBufferSubData)(GL_ARRAY_BUFFER, 0, new_size, &_buffer[0]);
    }
}

void Material::draw_triangles() const {
    if (!core::stats::debug_single_draw_call_enabled
        || core::stats::draw_calls.read() == core::stats::debug_single_draw_call_index) {
        GL_FUNC(glDrawArrays)(GL_TRIANGLES, 0, (int)_vertex_count);
    }
    core::stats::draw_calls.increment();
    core::stats::triangle_count.increment(_vertex_count/3);
}
