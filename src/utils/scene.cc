#include "rosewood/utils/scene.h"

#include "rosewood/core/memory.h"
#include "rosewood/core/transform.h"

#include "rosewood/math/matrix4.h"
#include "rosewood/math/matrix3.h"

#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/renderer.h"
#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/material.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/render_queue.h"

using rosewood::core::Entity;
using rosewood::core::Transform;
using rosewood::core::transform;

using rosewood::math::Matrix4;

using rosewood::graphics::Camera;
using rosewood::graphics::RenderQueue;
using rosewood::graphics::Renderer;
using rosewood::graphics::RenderCommand;
using rosewood::graphics::camera;
using rosewood::graphics::renderer;

using rosewood::utils::Scene;

void Scene::draw() {
    auto camera_comp = camera(_main_camera_node);
    
    if (!_queue) {
        _queue = make_unique<RenderQueue>();
    }

    _queue->clear();
    draw_camera(_queue.get(), camera_comp);
    _queue->sort();
    _queue->run();
}

void Scene::draw_camera(RenderQueue *queue, Camera *camera) {
    draw_tree(queue, camera, transform(_root_node),
              math::make_identity(), math::make_identity(),
              1);
}

void Scene::draw_tree(RenderQueue *queue, Camera *camera, Transform *tree,
                      const Matrix4 &acc_transform,
                      const Matrix4 &acc_inverse_transform,
                      float max_axis_scale) {
    auto root_transform = acc_transform * tree->local_transform();
    auto root_inv_transform = tree->inverse_local_transform() * acc_inverse_transform;
    auto scale = tree->local_scale();
    auto root_max_axis_scale = max_axis_scale * std::max({scale.x, scale.y, scale.z});

    for (auto child : tree->children()) {
        draw_tree(queue, camera, child,
                  root_transform, root_inv_transform,
                  root_max_axis_scale);
    }

    auto r = renderer(tree->entity());
    if (!r) return;

    queue->add_command(RenderCommand(r->mesh().get(),
                                     root_transform,
                                     root_inv_transform,
                                     root_max_axis_scale,
                                     r->material().get(),
                                     camera));
}
