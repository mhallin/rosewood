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
    
    class Camera : public core::Component<Camera> {
    public:
        Camera(core::Entity owner);
        
        void set_fov(float angles);
        void set_aspect(float aspect);
        void set_z_planes(float near, float far);

        float fov() const;
        float aspect() const;
        float z_near() const;
        float z_far() const;
        
        const ViewFrustum &view_frustum() const;
        
        math::Matrix4 projection_matrix() const;
        math::Matrix4 inverse_projection_matrix() const;
        
    private:
        float _fov;
        float _aspect;
        float _z_near, _z_far;
        
        ViewFrustum _view_frustum;
        
        void invalidate_frustum();
    };
    
    inline Camera *camera(core::Entity entity) {
        return entity.component<Camera>();
    }
    
    inline Camera::Camera(core::Entity owner) : core::Component<Camera>(owner), _view_frustum(this) { }
    
    inline void Camera::set_fov(float angles) { _fov = angles; invalidate_frustum(); }
    inline void Camera::set_aspect(float aspect) { _aspect = aspect; invalidate_frustum(); }
    inline void Camera::set_z_planes(float near, float far) {
        _z_near = near;
        _z_far = far;
        invalidate_frustum();
    }

    inline float Camera::fov() const { return _fov; }
    inline float Camera::aspect() const { return _aspect; }
    inline float Camera::z_near() const { return _z_near; }
    inline float Camera::z_far() const { return _z_far; }

    inline const ViewFrustum &Camera::view_frustum() const { return _view_frustum; }
    
    inline void Camera::invalidate_frustum() { _view_frustum = ViewFrustum(this); }
    
} }

#endif
