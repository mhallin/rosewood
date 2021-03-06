#ifndef __ROSEWOOD_MATH_MATH_UTILS_H__
#define __ROSEWOOD_MATH_MATH_UTILS_H__

#include <stdlib.h>
#include <algorithm>
#include <cmath>

namespace rosewood { namespace math {

    template<typename T>
    T clamp(T value, T min, T max) {
        return std::min(std::max(value, min), max);
    }

    template<typename T>
    T sign(T value) {
        return std::copysign(T(1), value);
    }

    inline float random(float min, float max) {
        return min + (max - min) * (static_cast<double>(::random()) / static_cast<double>(INT_MAX));
    }

    inline Vector3 random(Vector3 min, Vector3 max) {
        return Vector3(random(min.x(), max.x()), random(min.y(), max.y()), random(min.z(), max.z()));
    }

    template<typename T> T random_centered(T center, T diff) {
        return random(center - diff, center + diff);
    }

} }

#endif
