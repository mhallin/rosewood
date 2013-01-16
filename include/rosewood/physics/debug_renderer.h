#ifndef __ROSEWOOD_PHYSICS_DEBUG_RENDERER_H__
#define __ROSEWOOD_PHYSICS_DEBUG_RENDERER_H__

#include <vector>
#include <mutex>

#include "rosewood/core/memory.h"

#include "rosewood/graphics/platform_gl.h"

#include "rosewood/physics/bullet_common.h"

namespace rosewood { namespace graphics {
    class Shader;
    class Camera;
} }

namespace rosewood { namespace physics {

    class DebugRenderer : public btIDebugDraw {
    public:
        DebugRenderer(const std::shared_ptr<graphics::Shader> &shader,
                      graphics::Camera *camera);
        ~DebugRenderer();

        void start_update();
        void end_update();
        void draw();

        // btIDebugDraw Interface
        virtual void drawLine(const btVector3& from,
                              const btVector3& to,
                              const btVector3& color) override;

        virtual void drawContactPoint(const btVector3& point_on_b,
                                      const btVector3& normal_on_b,
                                      btScalar distance,
                                      int lifetime,
                                      const btVector3& color) override;

        virtual void reportErrorWarning(const char* warning_string) override;

        virtual void draw3dText(const btVector3& location, const char* text_string) override;

        virtual void setDebugMode(int debug_mode) override;

        virtual int getDebugMode() const override;

    private:
        int _debug_mode;
        std::vector<float> _line_vertex_data;
        int _line_count;

        std::shared_ptr<graphics::Shader> _shader;
        graphics::Camera *_camera;

        GLuint _vao;
        GLuint _vbo;

        std::mutex _draw_mutex;

        void upload_vbo_data();
        void draw_lines();

        void init_vao();
        void init_vbo();
    };

} }

#endif
