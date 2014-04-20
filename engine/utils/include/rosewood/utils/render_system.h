#ifndef __ROSEWOOD_ENGINE_SCENE_H__
#define __ROSEWOOD_ENGINE_SCENE_H__

#include <memory>
#include <mutex>

#include "rosewood/core/entity.h"

#include "rosewood/graphics/render_queue.h"

namespace rosewood { namespace utils {

    class RenderSystem {
    public:
        RenderSystem(core::EntityManager *entities, std::mutex *scene_mutex)
        : _entities(entities), _scene_mutex(scene_mutex) { }
        void draw();

    private:
        core::EntityManager *_entities;
        graphics::RenderQueue _queue;

        std::mutex *_scene_mutex;
    };

} }

#endif
