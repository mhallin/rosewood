#include "rosewood/graphics/render_queue.h"

#include <algorithm>

#include <iostream>

#include "rosewood/math/math_types.h"
#include "rosewood/math/matrix3.h"
#include "rosewood/math/matrix4.h"

#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/material.h"
#include "rosewood/graphics/gl_state.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/scene_object.h"
#include "rosewood/graphics/view_frustum.h"

using rosewood::graphics::RenderCommand;
using rosewood::graphics::RenderQueue;

bool rosewood::graphics::operator<(const RenderCommand &lhs, const RenderCommand &rhs) {
    int cmp = (int)(lhs._camera - rhs._camera);
    if (cmp) return cmp > 0;

    cmp = lhs._shader->queue_index() - rhs._shader->queue_index();
    if (cmp) return cmp > 0;

    cmp = (int)(lhs._shader - rhs._shader);
    if (cmp) return cmp > 0;

    cmp = (int)(lhs._material - rhs._material);
    if (cmp) return cmp > 0;

    return math::get(lhs._transform, 2, 3) < math::get(rhs._transform, 2, 3);
}

void RenderCommand::execute(const RenderCommand *previous) const {
    auto old_mat = previous ? previous->_material : nullptr;

    if (old_mat != _material) {
        if (old_mat) {
            previous->flush();
        }
        activate_material();
    }

    if (_camera->view_frustum().is_visible(_mesh, _transform, _max_axis_scale)) {
        _material->enqueue_mesh(_mesh, _transform, _inverse_transform);
    }
}

void RenderCommand::flush() const {
    if (_material->has_enqueued_meshes()) {
        auto shader = _material->shader();
        shader->set_projection_uniform(_camera->projection_matrix());
        shader->set_modelview_uniform(math::make_identity());
        shader->set_normal_uniform(mat3(math::make_identity()));
        shader->use();
        _material->submit_draw_calls();
    }
}

void RenderCommand::activate_material() const {
    _material->clear_vertex_buffer();
}

void RenderQueue::clear() {
    _commands.clear();
}

void RenderQueue::sort() {
    std::sort(begin(_commands), end(_commands));
}

void RenderQueue::run() {
    RenderCommand *prev = nullptr;
    for (auto &command : _commands) {
        command.execute(prev);
        prev = &command;
    }
    if (prev) prev->flush();
}
