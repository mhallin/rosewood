#ifndef __ROSEWOOD_GRAPHICS_VIEW_FRUSTUM_H__
#define __ROSEWOOD_GRAPHICS_VIEW_FRUSTUM_H__

#include <array>

#include "rosewood/math/math_types.h"

namespace rosewood { namespace graphics {
    
    class Mesh;
    class Camera;

    enum class FrustumPlane {
        Top, Bottom,
        Left, Right,
        Near, Far,
        NumPlanes
    };
    
    class ViewFrustum {
    public:
        explicit ViewFrustum(const Camera *camera);
        
        bool is_visible(const Mesh *mesh,
                        const math::Matrix4 &transform,
                        float max_axis_scale) const;
        
    private:
        math::Matrix4 _projection_matrix;

        std::array<math::Plane, (int)FrustumPlane::NumPlanes> _planes;
        
        void make_perspective_frustum_planes(const Camera *camera);
        void make_orthographic_frustum_planes(const Camera *camera);
    };
    
} }

#endif
