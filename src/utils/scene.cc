#include "rosewood/utils/scene.h"

#include "rosewood/core/memory.h"

#include "rosewood/graphics/scene_object.h"
#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/renderer.h"
#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/material.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/render_queue.h"

#include "rosewood/math/matrix4.h"
#include "rosewood/math/matrix3.h"

using rosewood::math::Matrix4;

using rosewood::graphics::Camera;
using rosewood::graphics::RenderQueue;
using rosewood::graphics::SceneObject;
using rosewood::graphics::Renderer;
using rosewood::graphics::RenderCommand;

using rosewood::utils::Scene;

void Scene::draw() {
    auto camera = _main_camera_node->component<Camera>();
    
    if (!_queue) {
        _queue = make_unique<RenderQueue>();
    }

    _queue->clear();
    draw_camera(_queue.get(), camera);
    _queue->sort();
    _queue->run();
}

void Scene::update() {
    if (_root_node) {
        _root_node->recursive_update_forward();
        _root_node->recursive_update_backward();
    }
}

void Scene::draw_camera(RenderQueue *queue, Camera *camera) {
    draw_tree(queue, camera, _root_node.get(),
              math::make_identity(), math::make_identity(),
              1);
}

void Scene::draw_tree(RenderQueue *queue, Camera *camera, SceneObject *tree,
                      const Matrix4 &acc_transform,
                      const Matrix4 &acc_inverse_transform,
                      float max_axis_scale) {
    auto root_transform = acc_transform * tree->local_transform();
    auto root_inv_transform = tree->inverse_local_transform() * acc_inverse_transform;
    auto scale = tree->local_scale();
    auto root_max_axis_scale = max_axis_scale * std::max({scale.x, scale.y, scale.z});

    for (auto &child : tree->children()) {
        draw_tree(queue, camera, child.get(),
                  root_transform, root_inv_transform,
                  root_max_axis_scale);
    }

    auto renderer = tree->component<Renderer>();
    if (!renderer) return;

    queue->add_command(RenderCommand(renderer->mesh().get(),
                                     root_transform,
                                     root_inv_transform,
                                     root_max_axis_scale,
                                     renderer->material().get(),
                                     camera));
}
