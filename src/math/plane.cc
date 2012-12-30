#include "rosewood/math/plane.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/vector.h"

namespace rosewood { namespace math {

    Plane::Plane() { }
    Plane::Plane(float x, float y, float z, float p) : x(x), y(y), z(z), p(p) { }

    Plane plane_from_points(Vector3 p1, Vector3 p2, Vector3 p3) {
        auto v1 = p1 - p2;
        auto v2 = p3 - p2;

        auto normal = normalized(cross(v2, v1));
        auto p = -dot(normal, p2);

        return Plane(normal.x, normal.y, normal.z, p);
    }

    float distance(Plane p, Vector3 v) {
        return p.p + dot(Vector3(p.x, p.y, p.z), v);
    }

    float distance(Vector3 v, Plane p) {
        return distance(p, v);
    }

} }
