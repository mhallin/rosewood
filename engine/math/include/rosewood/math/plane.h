#ifndef __ROSEWOOD_MATH_PLANE_H__
#define __ROSEWOOD_MATH_PLANE_H__

namespace rosewood { namespace math {

    struct Plane;
    struct Vector3;

    Plane plane_from_points(Vector3 p1, Vector3 p2, Vector3 p3);

    float distance(  Plane p, Vector3 v);
    float distance(Vector3 v,   Plane p);

} }

#endif
