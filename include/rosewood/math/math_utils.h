#ifndef __ROSEWOOD_MATH_MATH_UTILS_H__
#define __ROSEWOOD_MATH_MATH_UTILS_H__

#include <algorithm>

namespace rosewood { namespace math {

    template<typename T>
    T clampf(T value, T min, T max) {
        return std::min(std::max(value, min), max);
    }

} }

#endif
