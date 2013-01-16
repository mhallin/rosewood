#ifndef __ROSEWOOD_ENGINE_SCENE_H__
#define __ROSEWOOD_ENGINE_SCENE_H__

#include <memory>

#include "rosewood/core/entity.h"

namespace rosewood { namespace core {
    class Transform;
} }

namespace rosewood { namespace math {
    class Matrix4;
} }

namespace rosewood { namespace graphics {
    class Camera;
    class RenderQueue;
} }

namespace rosewood { namespace utils {

    class Scene {
    public:
        void draw();
        
        core::Entity root_node() const;
        
        void set_root_node(core::Entity node);
        void set_main_camera_node(core::Entity node);
    
    private:
        core::Entity _root_node;
        core::Entity _main_camera_node;
        
        std::unique_ptr<graphics::RenderQueue> _queue;
        
        void draw_camera(graphics::RenderQueue *queue, graphics::Camera *camera);
        static void draw_tree(graphics::RenderQueue *queue,
                              graphics::Camera *camera, core::Transform *tree,
                              const math::Matrix4 &acc_transform,
                              const math::Matrix4 &acc_inverse_transform,
                              float max_axis_scale);
    };
    
    inline core::Entity Scene::root_node() const {
        return _root_node;
    }

    inline void Scene::set_root_node(core::Entity node) {
        _root_node = node;
    }

    inline void Scene::set_main_camera_node(core::Entity node) {
        _main_camera_node = node;
    }

} }

#endif
