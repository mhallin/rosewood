#ifndef __ROSEWOOD_MATH_MATH_OSTREAM_H__
#define __ROSEWOOD_MATH_MATH_OSTREAM_H__

#include <ostream>

#include "rosewood/math/math_types.h"

namespace rosewood { namespace math {

    inline ::std::ostream &operator<<(::std::ostream &os, Vector3 v) {
        return (os << "Vector3(" << v.x() << ", " << v.y() << ", " << v.z() << ")");
    }
    
    inline ::std::ostream &operator<<(::std::ostream &os, Quaternion q) {
        return (os << "Quaternion(" << q._w << ", " << q._x << ", " << q._y << ", " << q._z << ")");
    }
    
} }
        
#endif
