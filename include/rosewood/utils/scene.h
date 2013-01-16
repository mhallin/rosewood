#ifndef __ROSEWOOD_ENGINE_SCENE_H__
#define __ROSEWOOD_ENGINE_SCENE_H__

#include <memory>
#include <mutex>

#include "rosewood/core/entity.h"

namespace rosewood { namespace graphics {
    class Camera;
    class RenderQueue;
} }

namespace rosewood { namespace utils {

    class Scene {
    public:
        void draw();
        
        core::Entity root_node() const { return _root_node; }
        
        void set_root_node(core::Entity node) { _root_node = node; }
        void set_main_camera_node(core::Entity node) { _main_camera_node = node; }
        
        std::mutex &scene_mutex() { return _scene_mutex; }
    
    private:
        core::Entity _root_node;
        core::Entity _main_camera_node;
        
        std::unique_ptr<graphics::RenderQueue> _queue;
        
        std::mutex _scene_mutex;
        
        void draw_camera(graphics::RenderQueue *queue, graphics::Camera *camera);
    };

} }

#endif
