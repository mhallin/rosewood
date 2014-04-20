#ifndef __ROSEWOOD_MATH_MATRIX3_H__
#define __ROSEWOOD_MATH_MATRIX3_H__

#include "math_types.h"

namespace rosewood { namespace math {

    // Matrix3 functions
    float   get       (const Matrix3 &m, size_t row, size_t col);
    void    set       (      Matrix3 &m, size_t row, size_t col, float value);

    Matrix3 transposed(const Matrix3 &m);

    bool    operator==(const Matrix3 &lhs, const Matrix3 &rhs);
    bool    operator!=(const Matrix3 &lhs, const Matrix3 &rhs);
    
    Vector3 operator* (const Matrix3 &lhs, const Vector3 &rhs);
    

    // Matrix3 function implementations
    inline const float *ptr  (const Matrix3 &m) { return m._m.data(); }
    inline       float *ptr  (      Matrix3 &m) { return m._m.data(); }

    inline const float *begin(const Matrix3 &m) { return begin(m._m); }
    inline const float *end  (const Matrix3 &m) { return end(m._m);   }

} }

#endif
