#ifndef __ROSEWOOD_ENGINE_SCENE_H__
#define __ROSEWOOD_ENGINE_SCENE_H__

#include <memory>

namespace rosewood { namespace math {
    class Matrix4;
} }

namespace rosewood { namespace graphics {
    class SceneObject;
    class Camera;
    class RenderQueue;
} }

namespace rosewood { namespace utils {

    class Scene {
    public:
        void draw();
        void update();
        
        std::shared_ptr<graphics::SceneObject> root_node() const;
        
        void set_root_node(std::shared_ptr<graphics::SceneObject> node);
        void set_main_camera_node(std::shared_ptr<graphics::SceneObject> node);
    
    private:
        std::shared_ptr<graphics::SceneObject> _root_node;
        std::shared_ptr<graphics::SceneObject> _main_camera_node;
        
        std::unique_ptr<graphics::RenderQueue> _queue;
        
        void draw_camera(graphics::RenderQueue *queue, graphics::Camera *camera);
        static void draw_tree(graphics::RenderQueue *queue,
                              graphics::Camera *camera, graphics::SceneObject *tree,
                              const math::Matrix4 &acc_transform,
                              const math::Matrix4 &acc_inverse_transform,
                              float max_axis_scale);
    };
    
    inline std::shared_ptr<graphics::SceneObject> Scene::root_node() const {
        return _root_node;
    }

    inline void Scene::set_root_node(std::shared_ptr<graphics::SceneObject> node) {
        _root_node = node;
    }

    inline void Scene::set_main_camera_node(std::shared_ptr<graphics::SceneObject> node) {
        _main_camera_node = node;
    }

} }

#endif
