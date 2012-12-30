#ifndef __ROSEWOOD_EXAMPLE_SAMPLE_APP_H__
#define __ROSEWOOD_EXAMPLE_SAMPLE_APP_H__

#include <memory>

namespace rosewood { namespace graphics {
    class Camera;
    class SceneObject;
} }

namespace rosewood { namespace utils {
    class Scene;
} }

namespace sample_app {

    using rosewood::graphics::Camera;
    using rosewood::graphics::SceneObject;

    using rosewood::utils::Scene;

    class RosewoodApp {
    public:
        static RosewoodApp *create();
        static void destroy(RosewoodApp *app);
        
        void update();
        void draw() const;

        void reshape_viewport(float width, float height);

    private:
        RosewoodApp();
        std::unique_ptr<Scene> _scene;
        Camera *_main_camera;

        std::shared_ptr<SceneObject> _cube1;
        std::shared_ptr<SceneObject> _cube2;

        float _rotation;
    };

}

#endif
