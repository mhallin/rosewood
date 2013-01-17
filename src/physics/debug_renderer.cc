#include "rosewood/physics/debug_renderer.h"

#include <iostream>

#include "rosewood/core/transform.h"

#include "rosewood/math/matrix4.h"
#include "rosewood/math/matrix3.h"

#include "rosewood/graphics/gl_func.h"
#include "rosewood/graphics/gl_state.h"
#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/camera.h"

using rosewood::math::from_bt;

using rosewood::core::transform;

using rosewood::graphics::Shader;
using rosewood::graphics::Camera;

using rosewood::physics::DebugRenderer;

DebugRenderer::DebugRenderer(const std::shared_ptr<Shader> &shader, Camera *camera)
: _line_count(0)
, _shader(shader)
, _camera(camera)
, _vao(UINT_MAX)
, _vbo(UINT_MAX)
{
}

DebugRenderer::~DebugRenderer() {
    if (_vbo != UINT_MAX) {
        GL_FUNC(glDeleteBuffers)(1, &_vbo);
    }
    if (_vao != UINT_MAX) {
        GL_FUNC(glDeleteVertexArrays)(1, &_vao);
    }
}

void DebugRenderer::start_update() {
    _draw_mutex.lock();
    _line_vertex_data.clear();
    _line_count = 0;
}

void DebugRenderer::end_update() {
    _draw_mutex.unlock();
}

void DebugRenderer::draw() {
    if (_vbo == UINT_MAX) init_vbo();
    if (_vao == UINT_MAX) init_vao();

    graphics::gl_state::bind_vertex_array_object(_vao);
    graphics::gl_state::bind_array_buffer(_vbo);
    graphics::gl_state::disable(GL_DEPTH_TEST);

    {
        std::lock_guard<std::mutex> lock(_draw_mutex);
        upload_vbo_data();
        draw_lines();
    }

    graphics::gl_state::enable(GL_DEPTH_TEST);
}

void DebugRenderer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    auto tform = transform(_camera->entity());
    auto v1 = math::make_hand_shift() * tform->inverse_world_transform() * from_bt(from);
    auto v2 = math::make_hand_shift() * tform->inverse_world_transform() * from_bt(to);
//    auto v1 = from_bt(from), v2 = from_bt(to);

    float vertex_data[24] = {
        v1.x, v1.y, v1.z, // rw_vertex
        0, 0, 0, // rw_normal
        0, 0, // rw_texcoord
        color.x(), color.y(), color.z(), 1, // color
        v2.x, v2.y, v2.z, // rw_vertex
        0, 0, 0, // rw_normal
        0, 0, // rw_texcoord
        color.x(), color.y(), color.z(), 1 // color
    };

    _line_vertex_data.insert(end(_line_vertex_data), std::begin(vertex_data), std::end(vertex_data));
    ++_line_count;
}

void DebugRenderer::drawContactPoint(const btVector3 &point_on_b, const btVector3 &normal_on_b,
                                     btScalar distance, int lifetime, const btVector3 &color) {

}

void DebugRenderer::reportErrorWarning(const char *warning_string) {
    std::cerr << "BULLET WARNING: " << warning_string << "\n";
}

void DebugRenderer::draw3dText(const btVector3 &location, const char *text_string) {

}

void DebugRenderer::setDebugMode(int debug_mode) {
    _debug_mode = debug_mode;
}

int DebugRenderer::getDebugMode() const {
    return _debug_mode;
}

void DebugRenderer::init_vbo() {
    GL_FUNC(glGenBuffers)(1, &_vbo);
}

void DebugRenderer::init_vao() {
    GL_FUNC(glGenVertexArrays)(1, &_vao);

    graphics::gl_state::bind_vertex_array_object(_vao);
    graphics::gl_state::bind_array_buffer(_vbo);

    _shader->initialize_attribute_arrays();
}

void DebugRenderer::upload_vbo_data() {
    auto size = _line_vertex_data.size() * sizeof(float);

    GL_FUNC(glBufferData)(GL_ARRAY_BUFFER, size, &_line_vertex_data[0], GL_STATIC_DRAW);
}

void DebugRenderer::draw_lines() {
    _shader->use();
//    _shader->set_modelview_uniform(_camera->owner()->inverse_world_transform());
    _shader->set_modelview_uniform(math::make_identity());
    _shader->set_normal_uniform(mat3(math::make_identity()));
    _shader->set_projection_uniform(_camera->projection_matrix());

    GL_FUNC(glDrawArrays)(GL_LINES, 0, _line_count * 2);
}
