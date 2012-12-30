#include "rosewood/graphics/view_frustum.h"

#include <algorithm>

#include "rosewood/math/vector.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/plane.h"

#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/mesh.h"

using rosewood::math::Vector3;
using rosewood::math::Vector4;
using rosewood::math::Matrix4;
using rosewood::math::plane_from_points;
using rosewood::math::distance;

using rosewood::graphics::Camera;
using rosewood::graphics::Mesh;
using rosewood::graphics::ViewFrustum;

ViewFrustum::ViewFrustum(const Camera *camera)
: _projection_matrix(camera->projection_matrix()) {
    Vector3 forward(0, 0, -1);
    Vector3 up(0, 1, 0);
    Vector3 right(1, 0, 0);

    float far_height = 2 * tanf(camera->fov() / 2.0f) * camera->z_far();
    float far_width = far_height * camera->aspect();
    Vector3 far_center = forward * camera->z_far();
    Vector3 far_tl = far_center + (up * far_height/2) - (right * far_width/2);
    Vector3 far_tr = far_center + (up * far_height/2) + (right * far_width/2);
    Vector3 far_bl = far_center - (up * far_height/2) - (right * far_width/2);
    Vector3 far_br = far_center - (up * far_height/2) + (right * far_width/2);

    float near_height = 2 * tanf(camera->fov() / 2.0f) * camera->z_near();
    float near_width = near_height * camera->aspect();
    Vector3 near_center = forward * camera->z_near();
    Vector3 near_tl = near_center + (up * near_height/2) - (right * near_width/2);
    Vector3 near_tr = near_center + (up * near_height/2) + (right * near_width/2);
    Vector3 near_bl = near_center - (up * near_height/2) - (right * near_width/2);
    Vector3 near_br = near_center - (up * near_height/2) + (right * near_width/2);

    _planes[(int)FrustumPlane::Top] = plane_from_points(near_tr, near_tl, far_tl);
    _planes[(int)FrustumPlane::Bottom] = plane_from_points(near_bl, near_br, far_br);
    _planes[(int)FrustumPlane::Left] = plane_from_points(near_tl, near_bl, far_bl);
    _planes[(int)FrustumPlane::Right] = plane_from_points(near_br, near_tr, far_br);
    _planes[(int)FrustumPlane::Near] = plane_from_points(near_tl, near_tr, near_br);
    _planes[(int)FrustumPlane::Far] = plane_from_points(far_tr, far_tl, far_bl);
}

bool ViewFrustum::is_visible(const Mesh *mesh,
                             const Matrix4 &transform,
                             float max_axis_scale) const {
    auto center = Vector3(transform * Vector4(0, 0, 0, 1));
    auto mradius2 = mesh->bounding_sphere_radius2();
    auto radius2 = max_axis_scale * max_axis_scale * mradius2;

    for (int p = 0; p < (int)FrustumPlane::NumPlanes; ++p) {
        auto dist = distance(_planes[p], center);
        if (copysign(dist * dist, dist) < -radius2) {
            return false;
        }
    }

    return true;
    
//    auto &points = mesh->vertex_data();
//    auto pm = _projection_matrix * transform;
//    
//    return std::any_of(begin(points), end(points), [=](Vector3 p) {
//        return is_proj_point_visible(pm * p);
//    });
}

bool ViewFrustum::is_proj_point_visible(Vector3 proj_point) const {
    return (proj_point.x >= -1 && proj_point.x <= 1 &&
            proj_point.y >= -1 && proj_point.y <= 1 &&
            proj_point.z >= -1 && proj_point.z <= 1);
}
