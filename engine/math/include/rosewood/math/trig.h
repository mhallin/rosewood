#ifndef __ROSEWOOD_MATH_TRIG_H__
#define __ROSEWOOD_MATH_TRIG_H__

#include <math.h>

#include "rosewood/math/vector.h"

namespace rosewood { namespace math {

    // rad = deg * (M_PI/180)
    inline constexpr float deg2rad(float deg) {
        return deg * ((float)M_PI/180.0f);
    }

    // deg = rad * (180/M_PI)
    inline constexpr float rad2deg(float rad) {
        return rad * (180.0f/(float)M_PI);
    }

    inline float cotf(float x) {
        return cosf(x)/sinf(x);
    }

    inline float angle_between(const Vector3 &v1, const Vector3 &v2) {
        auto s = length(cross(v1, v2));
        auto c = dot(v1, v2);
        return atan2f(s, c);
    }

    inline float signed_angle_between(const Vector3 &v1, const Vector3 &v2, const Vector3 &up) {
        auto a = angle_between(v1, v2);
        auto v3 = cross(up, v1);
        return copysignf(a, dot(v3, v2));
    }

} }

#endif
