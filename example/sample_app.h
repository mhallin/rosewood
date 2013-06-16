#ifndef __ROSEWOOD_EXAMPLE_SAMPLE_APP_H__
#define __ROSEWOOD_EXAMPLE_SAMPLE_APP_H__

#include "rosewood/core/entity.h"

#include "rosewood/utils/render_system.h"

#include <memory>

namespace rosewood { namespace graphics {
    class Camera;
} }

namespace sample_app {

    class RosewoodApp {
    public:
        static RosewoodApp *create();
        static void destroy(RosewoodApp *app);
        
        void update();
        void draw();

        void reshape_viewport(float width, float height);

    private:
        RosewoodApp();

        std::mutex _scene_mutex;

        rosewood::core::EntityManager _entity_manager;
        rosewood::graphics::Camera *_main_camera;
        rosewood::utils::RenderSystem _render_system;

        rosewood::core::Entity _root;
        rosewood::core::Entity _cube1;
        rosewood::core::Entity _cube2;

        float _rotation;
    };

}

#endif
