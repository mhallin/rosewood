#include "rosewood/graphics/material.h"

#include <ostream>

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

Material::Material() : _vbo(UINT_MAX), _vao(UINT_MAX), _last_size(0), _vertex_index(0) { }

Material::~Material() {
    if (_vbo != UINT_MAX) {
        GL_FUNC(glDeleteBuffers)(1, &_vbo);
    }
    if (_vao != UINT_MAX) {
        GL_FUNC(glDeleteVertexArrays)(1, &_vao);
    }
}

void Material::clear_vertex_buffer() {
    _vertex_index = 0;
}

void Material::enqueue_mesh(const Mesh *mesh, Matrix4 transform, Matrix4 inverse_transform) {
    auto nverts = mesh->vertex_data().size() * shader()->attribute_stride() / sizeof(float);
    if (_vertex_index + nverts > _vertices.size()) {
        _vertices.resize(_vertex_index + nverts);
    }
    mesh->instantiate(transform, inverse_transform, _vertices.begin() + _vertex_index,
                      shader()->extra_attributes());
    _vertex_index += nverts;
}

void Material::submit_draw_calls() {
    if (!_vertex_index) return;
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
    os << std::string(indent, ' ') << "  VAO: " << _vao << ", VBO: " << _vbo << ", Vertices: " << _vertices.size() << "\n";
    
    os << std::string(indent, ' ') << "  [";
    std::copy(begin(_vertices), end(_vertices), std::ostream_iterator<float>(os, ", "));
    os << "]\n";
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
    auto new_size = _vertex_index * sizeof(float);

    if (new_size > _last_size) {
        GL_FUNC(glBufferData)(GL_ARRAY_BUFFER, new_size, &_vertices[0], GL_DYNAMIC_DRAW);
        _last_size = new_size;
    }
    else {
        GL_FUNC(glBufferSubData)(GL_ARRAY_BUFFER, 0, new_size, &_vertices[0]);
    }
}

void Material::draw_triangles() const {
    auto stride = shader()->attribute_stride() / sizeof(float);
    int vertex_count = (int)_vertex_index/stride;

    GL_FUNC(glDrawArrays)(GL_TRIANGLES, 0, vertex_count);
    core::stats::draw_calls.increment();
    core::stats::triangle_count.increment(vertex_count/3);
}
