#ifndef __ROSEWOOD_MATH_MATRIX4_H__
#define __ROSEWOOD_MATH_MATRIX4_H__

#include "math_types.h"

namespace rosewood { namespace math {
    
    // Matrix4 functions
    Matrix4  make_identity     ();
    Matrix4  make_hand_shift   ();
    Matrix4  make_perspective  (float fovy, float aspect, float z_near, float z_far);
    Matrix4  make_inverse_perspective(float fovy, float aspect, float z_near, float z_far);
    Matrix4  make_translation  (Vector3 v);
    Matrix4  make_rotation     (Quaternion q);
    Matrix4  make_scale        (Vector3 v);
    

    float    get               (const Matrix4 &m, size_t row, size_t col);
    void     set               (      Matrix4 &m, size_t row, size_t col, float value);
    
    Matrix4  transposed        (const Matrix4 &m);
    
    Matrix3  mat3              (const Matrix4 &m);

    void     apply_translate_by(      Matrix4 &m, Vector3 v);
    void     apply_rotate_by   (      Matrix4 &m, Quaternion q);
    void     apply_scale_by    (      Matrix4 &m, Vector3 v);

    Matrix4 &operator*=        (      Matrix4 &lhs, const Matrix4 &rhs);
    Matrix4  operator*         (const Matrix4 &lhs, const Matrix4 &rhs);
    bool     operator==        (const Matrix4 &lhs, const Matrix4 &rhs);
    bool     operator!=        (const Matrix4 &lhs, const Matrix4 &rhs);

    Vector4  operator*         (const Matrix4 &lhs, const Vector4 &rhs);
    Vector3  operator*         (const Matrix4 &lhs, const Vector3 &rhs);
    
    // Matrix4 function implementations
    inline const float *ptr(const Matrix4 &m)   { return m._m.data(); }
    inline       float *ptr(      Matrix4 &m)   { return m._m.data(); }

    inline const float *begin(const Matrix4 &m) { return m._m.begin(); }
    inline const float *end  (const Matrix4 &m) { return m._m.end();   }
    
} }

#endif
