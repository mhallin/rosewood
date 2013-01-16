#ifndef __ROSEWOOD_MATH_VECTOR3_H__
#define __ROSEWOOD_MATH_VECTOR3_H__

#include <math.h>

#include "math_types.h"

namespace rosewood { namespace math {
    
    // Vector3 functions
    float         length    (const Vector3   v);
    float         length2   (const Vector3   v);
    float         dot       (const Vector3 lhs, const Vector3 rhs);
    Vector3       cross     (const Vector3 lhs, const Vector3 rhs);
    Vector3       emult     (const Vector3 lhs, const Vector3 rhs);
    
    Vector3       normalized(const Vector3   v);
    
    float         get       (const Vector3   v, size_t index);
    void          set       (      Vector3  &v, size_t index, float value);
    
    const float  *ptr       (const Vector3  &v);
    float        *ptr       (      Vector3  &v);

    Vector3       operator- (const Vector3   v);
    Vector3       operator/ (const Vector3   v, float s);
    
    bool          operator==(const Vector3 lhs, const Vector3 rhs);
    bool          operator!=(const Vector3 lhs, const Vector3 rhs);
    
    // Vector4 functions
    float         length    (const Vector4   v);
    float         length2   (const Vector4   v);
    float         dot       (const Vector4 lhs, const Vector4 rhs);
    
    Vector4       normalized(const Vector4  v);
    
    float         get       (const Vector4   v, size_t index);
    void          set       (      Vector4  &v, size_t index, float value);
    
    const float  *ptr       (const Vector4  &v);
    float        *ptr       (      Vector4  &v);
    
    Vector4       operator- (const Vector4   v);
    Vector4       operator/ (const Vector4   v, float s);
    
    // Vector2 function implementations
    inline         Vector2::Vector2()                   : x(0), y(0) { }
    inline         Vector2::Vector2(float x_, float y_) : x(x_), y(y_) { }
    
    // Vector3 function implementations
    inline         Vector3::Vector3()                                     : x(0), y(0), z(0) { }
    inline         Vector3::Vector3(float x_, float y_, float z_)         : x(x_), y(y_), z(z_) { }
    inline         Vector3::Vector3(const Vector4   v)                    : x(v.x/v.w), y(v.y/v.w), z(v.z/v.w) { }
    inline float   length          (const Vector3   v)                    { return sqrtf(length2(v)); }
    inline float   length2         (const Vector3   v)                    { return dot(v, v); }
    inline float   dot             (const Vector3 lhs, const Vector3 rhs) { return (lhs.x*rhs.x +
                                                                                    lhs.y*rhs.y +
                                                                                    lhs.z*rhs.z); }
    inline Vector3 cross           (const Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.y*rhs.z - lhs.z*rhs.y,
                                                                                           lhs.z*rhs.x - lhs.x*rhs.z,
                                                                                           lhs.x*rhs.y - lhs.y*rhs.x); }
    inline Vector3 emult           (const Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.x*rhs.x,
                                                                                           lhs.y*rhs.y,
                                                                                           lhs.z*rhs.z); }
    inline Vector3 normalized      (const Vector3   v)                    { return v/length(v); }

    inline float   get             (const Vector3   v, size_t i)          { return ptr(v)[i]; }
    inline void    set             (      Vector3  &v, size_t i, float f) { ptr(v)[i] = f; }
    
    inline const float *ptr        (const Vector3  &v)                    { return &v.x; }
    inline       float *ptr        (      Vector3  &v)                    { return &v.x; }

    inline Vector3 operator-       (const Vector3   v)                    { return Vector3(-v.x,
                                                                                           -v.y,
                                                                                           -v.z); }
    inline Vector3 operator+       (const Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.x+rhs.x,
                                                                                           lhs.y+rhs.y,
                                                                                           lhs.z+rhs.z); }
    inline Vector3 operator-       (const Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.x-rhs.x,
                                                                                           lhs.y-rhs.y,
                                                                                           lhs.z-rhs.z); }
    inline Vector3 operator*       (const Vector3   v, float s)           { return Vector3(v.x*s,
                                                                                           v.y*s,
                                                                                           v.z*s); }
    inline Vector3 operator/       (const Vector3   v, float s)           { return Vector3(v.x/s,
                                                                                           v.y/s,
                                                                                           v.z/s); }
    inline Vector3 operator/       (float s, const Vector3   v)           { return Vector3(s/v.x,
                                                                                           s/v.y,
                                                                                           s/v.z); }
    
    inline bool    operator==      (const Vector3 lhs, const Vector3 rhs) { return (lhs.x == rhs.x &&
                                                                                    lhs.y == rhs.y &&
                                                                                    lhs.z == rhs.z); }
    inline bool    operator!=      (const Vector3 lhs, const Vector3 rhs) { return !(lhs == rhs); }

    // Vector4 function implementations
    inline         Vector4::Vector4()                                       : x(0), y(0), z(0), w(0) { }
    inline         Vector4::Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) { }
    inline         Vector4::Vector4(const Vector3   v)                      : x(v.x), y(v.y), z(v.z), w(1) { }
    inline float   length          (const Vector4   v)                      { return sqrtf(length2(v)); }
    inline float   length2         (const Vector4   v)                      { return dot(v, v); }
    inline float   dot             (const Vector4 lhs, const Vector4 rhs)   { return (lhs.x*rhs.x +
                                                                                      lhs.y*rhs.y +
                                                                                      lhs.z*rhs.z +
                                                                                      lhs.w*rhs.w); }
    inline Vector4 normalized      (const Vector4   v)                      { return v/length(v); }
    
    inline float   get             (const Vector4   v, size_t i)            { return ptr(v)[i]; }
    inline void    set             (      Vector4  &v, size_t i, float f)   { ptr(v)[i] = f; }
    
    inline const float *ptr        (const Vector4  &v)                      { return &v.x; }
    inline       float *ptr        (      Vector4  &v)                      { return &v.x; }
    
    inline Vector4 operator-       (const Vector4   v)                      { return Vector4(-v.x,
                                                                                             -v.y,
                                                                                             -v.z,
                                                                                             -v.w); }
    inline Vector4 operator/       (const Vector4   v, float s)             { return Vector4(v.x/s,
                                                                                             v.y/s,
                                                                                             v.z/s,
                                                                                             v.w/s); }

} }

#endif
