#include "rosewood/utils/render_system.h"

#include "rosewood/core/memory.h"
#include "rosewood/core/transform.h"

#include "rosewood/math/vector.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/matrix3.h"

#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/renderable.h"
#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/material.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/render_queue.h"
#include "rosewood/graphics/light.h"

using rosewood::core::EntityManager;
using rosewood::core::Transform;
using rosewood::core::transform;
using rosewood::core::ComponentArrayView;

using rosewood::math::Matrix4;

using rosewood::graphics::Camera;
using rosewood::graphics::RenderQueue;
using rosewood::graphics::Renderable;
using rosewood::graphics::RenderCommand;
using rosewood::graphics::Light;
using rosewood::graphics::camera;

using rosewood::utils::RenderSystem;

static void draw_camera(EntityManager *entities, RenderQueue *queue, Camera *camera) {
    auto first_light = *entities->components<Light>().begin();

    entities->for_components<Renderable, Transform>(
        [=](Renderable *renderable, Transform *transform) {
            if (!renderable->enabled()) return;

            auto scale = transform->local_scale();
            queue->add_command(RenderCommand(renderable->mesh().get(),
                                             transform->world_transform(),
                                             transform->inverse_world_transform(),
                                             std::max({scale.x(), scale.y(), scale.z()}),
                                             renderable->material().get(),
                                             camera,
                                             first_light));
        });
}

void RenderSystem::draw() {
    _queue.clear();

    std::lock_guard<std::mutex> lock(*_scene_mutex);

    _entities->for_components<Camera>([this](Camera *camera) {
        draw_camera(_entities, &_queue, camera);
    });

    _queue.sort();
    _queue.run();
}
