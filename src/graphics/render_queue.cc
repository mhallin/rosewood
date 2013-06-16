#include "rosewood/graphics/render_queue.h"

#include <algorithm>

#include <iostream>

#include "rosewood/core/stats.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/matrix3.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/quaternion.h"
#include "rosewood/math/vector.h"

#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/material.h"
#include "rosewood/graphics/gl_state.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/view_frustum.h"
#include "rosewood/graphics/light.h"
#include "rosewood/graphics/texture.h"

using rosewood::core::transform;

using rosewood::graphics::RenderCommand;
using rosewood::graphics::RenderQueue;

bool rosewood::graphics::operator<(const RenderCommand &lhs, const RenderCommand &rhs) {
    int cmp = (int)(lhs._camera - rhs._camera);
    if (cmp) return cmp < 0;

    cmp = lhs._shader->queue_index() - rhs._shader->queue_index();
    if (cmp) return cmp < 0;

    auto lhs_tex_index = lhs._material->texture()
        ? lhs._material->texture()->index() : 0;
    auto rhs_tex_index = rhs._material->texture()
        ? rhs._material->texture()->index() : 0;

    cmp = (int)(lhs_tex_index - rhs_tex_index);
    if (cmp) return cmp < 0;

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

    _material->enqueue_mesh(_mesh, _transform, _inverse_transform);
}

bool RenderCommand::is_visible() const {
    return _camera->view_frustum().is_visible(_mesh, _transform, _max_axis_scale);
}

void RenderCommand::flush() const {
    if (_material->has_enqueued_meshes()) {
        auto shader = _material->shader();
        shader->set_projection_uniform(_camera->projection_matrix());
        shader->set_modelview_uniform(math::make_identity4());
        shader->set_normal_uniform(mat3(math::make_identity4()));

        if (_light) {
            auto light_mat = transform(_light->entity())->world_transform();
            auto inv_camera_mat = transform(_camera->entity())->inverse_world_transform();
            auto light_dir = (inv_camera_mat * light_mat) * math::Vector4(0, 0, 1, 0);

            shader->set_light_position_uniform(math::Vector3(light_dir.x, light_dir.y, light_dir.z));
            shader->set_light_color_uniform(_light->color());
        }
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
    core::stats::render_queue_size = _commands.size();

    RenderCommand *prev = nullptr;
    for (auto &command : _commands) {
        command.execute(prev);
        prev = &command;
    }
    if (prev) prev->flush();
}
