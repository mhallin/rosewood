#ifndef __ROSEWOOD_MATH_QUATERNION_H__
#define __ROSEWOOD_MATH_QUATERNION_H__

#include "math_types.h"

namespace rosewood { namespace math {

    // Quaternion functions
    Quaternion  quaternion_identity();
    Quaternion  quaternion_from_axis_angle(const Vector3 axis, float angle);
    Quaternion  quaternion_from_euler_angles(float x, float y, float z);

    
    Quaternion  inverse    (const Quaternion q);
    Quaternion  conjugate  (const Quaternion q);
    Matrix4     mat4       (const Quaternion q);
    
    float       norm       (const Quaternion q);
    float       dot        (const Quaternion lhs, const Quaternion rhs);
    
    Quaternion  operator-  (const Quaternion q);
    Quaternion  operator+  (const Quaternion lhs, const Quaternion rhs);
    Quaternion  operator-  (const Quaternion lhs, const Quaternion rhs);
    Quaternion  operator*  (const Quaternion lhs, const Quaternion rhs);
    Quaternion  operator*  (const Quaternion lhs, float rhs);
    Quaternion  operator*  (float lhs, const Quaternion rhs);
    Quaternion  operator/  (const Quaternion lhs, float rhs);
    
    Quaternion  normalized (const Quaternion q);

    
    // Quaternion function implementations
    inline Quaternion::Quaternion() { *this = quaternion_identity(); }
    inline Quaternion::Quaternion(float w, float x, float y, float z)
    : _w(w), _x(x), _y(y), _z(z) { }

    inline Quaternion inverse(const Quaternion q) { return Quaternion(-q._w, -q._x, -q._y, -q._z); }
    inline Quaternion conjugate(const Quaternion q) { return Quaternion(q._w, -q._x, -q._y, -q._z); }
    
    inline Quaternion quaternion_identity() { return Quaternion(1, 0, 0, 0); }

    inline Quaternion operator-(const Quaternion q) { return inverse(q); }
    
    inline Quaternion operator+(const Quaternion lhs, const Quaternion rhs) {
        return Quaternion(lhs._w+rhs._w, lhs._x+rhs._x, lhs._y+rhs._y, lhs._z+rhs._z);
    }
    
    inline Quaternion operator-(const Quaternion lhs, const Quaternion rhs) {
        return Quaternion(lhs._w-rhs._w, lhs._x-rhs._x, lhs._y-rhs._y, lhs._z-rhs._z);
    }
    
    inline Quaternion operator*(const Quaternion q, float s) { return Quaternion(q._w*s, q._x*s, q._y*s, q._z*s); }
    inline Quaternion operator*(float s, const Quaternion q) { return q * s; }
    inline Quaternion operator/(const Quaternion q, float s) { return q * (1.0f/s); }
    
    inline Quaternion normalized(const Quaternion q) { return q / norm(q); }

} }

#endif
