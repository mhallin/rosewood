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
using rosewood::core::ComponentArrayView;

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
    
    {
        std::lock_guard<std::mutex> lock(_scene_mutex);
        draw_camera(_queue.get(), camera_comp);
    }

    _queue->sort();
    _queue->run();
}

void Scene::draw_camera(RenderQueue *queue, Camera *camera) {
    for (auto renderer : _root_node.owner->components<Renderer>()) {
        auto tform = transform(renderer->entity());
        auto scale = tform->local_scale();
        queue->add_command(RenderCommand(renderer->mesh().get(),
                                         tform->world_transform(),
                                         tform->inverse_world_transform(),
                                         std::max({scale.x, scale.y, scale.z}),
                                         renderer->material().get(),
                                         camera));
    }
}
