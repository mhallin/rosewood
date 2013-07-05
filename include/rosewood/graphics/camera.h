#ifndef __ROSEWOOD_GRAPHICS_CAMERA_H__
#define __ROSEWOOD_GRAPHICS_CAMERA_H__

#include "rosewood/core/entity.h"
#include "rosewood/core/component.h"

#include "view_frustum.h"

namespace rosewood { namespace math {
    class Matrix4;
} }

namespace rosewood { namespace graphics {
    
    class Camera;
    
    Camera *camera(core::Entity entity);

    enum class ProjectionMode {
        kPerspectiveMode,
        kOrthographicMode,
    };

    class Camera : public core::Component<Camera> {
    public:
        Camera(core::Entity owner);
        
        void set_mode(ProjectionMode mode) { _mode = mode; invalidate_frustum(); }

        void set_fov(float angles) { _fov = angles; invalidate_frustum(); }
        void set_aspect(float aspect) { _aspect = aspect; invalidate_frustum(); }

        void set_width(float width) { _width = width; invalidate_frustum(); }
        void set_height(float height) { _height = height; invalidate_frustum(); }

        void set_z_planes(float near, float far) { _z_near = near; _z_far = far; invalidate_frustum(); }

        ProjectionMode mode() const { return _mode; }

        float fov() const { return _fov; }
        float aspect() const { return _aspect; }

        float width() const { return _width; }
        float height() const { return _height; }

        float z_near() const { return _z_near; }
        float z_far() const { return _z_far; }

        const ViewFrustum &view_frustum() const { return _view_frustum; }
        
        math::Matrix4 projection_matrix() const;
        math::Matrix4 inverse_projection_matrix() const;
        
        math::Vector3 projection_to_world(math::Vector3 proj_point) const;
        
    private:
        union {
            struct {
                float _fov;
                float _aspect;
            };
            struct {
                float _width;
                float _height;
            };
        };

        float _z_near, _z_far;

        ProjectionMode _mode;

        ViewFrustum _view_frustum;
        
        void invalidate_frustum();
    };
    
    inline Camera *camera(core::Entity entity) {
        return entity.component<Camera>();
    }

    inline Camera::Camera(core::Entity owner) : core::Component<Camera>(owner), _view_frustum(this) { };
    
    inline void Camera::invalidate_frustum() { _view_frustum = ViewFrustum(this); }
    
} }

#endif
