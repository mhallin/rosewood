#ifndef __ROSEWOOD_MATH_TRIG_H__
#define __ROSEWOOD_MATH_TRIG_H__

#include <math.h>

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

} }

#endif
