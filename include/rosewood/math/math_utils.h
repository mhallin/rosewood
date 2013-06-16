#ifndef __ROSEWOOD_MATH_MATH_UTILS_H__
#define __ROSEWOOD_MATH_MATH_UTILS_H__

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

} }

#endif
