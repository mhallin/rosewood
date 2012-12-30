#ifndef __ROSEWOOD_MATH_MATH_TYPES_H__
#define __ROSEWOOD_MATH_MATH_TYPES_H__

#include <array>

namespace rosewood { namespace math {
    
    struct Matrix4;
    struct Matrix3;
    struct Vector4;
    struct Vector3;
    struct Quaternion;
    struct Plane;

    struct Matrix4 {
        static const size_t kSize = 16;
        
        Matrix4();
        Matrix4(float m11, float m12, float m13, float m14,
                float m21, float m22, float m23, float m24,
                float m31, float m32, float m33, float m34,
                float m41, float m42, float m43, float m44);

        std::array<float, kSize> _m;
    };

    struct Matrix3 {
        static const size_t kSize = 9;

        Matrix3();
        Matrix3(float m11, float m12, float m13,
                float m21, float m22, float m23,
                float m31, float m32, float m33);

        std::array<float, kSize> _m;
    };
    
    struct Vector4 {
        Vector4();
        Vector4(float x, float y, float z, float w);
        explicit Vector4(const Vector3 v);
        
        float x, y, z, w;
    };

    struct Vector3 {
        Vector3();
        Vector3(float x, float y, float z);
        explicit Vector3(const Vector4 v);
        
        float x, y, z;
    };
    
    struct Vector2 {
        Vector2();
        Vector2(float x, float y);
        
        float x, y;
    };
    
    struct Quaternion {
        Quaternion();
        Quaternion(float w, float x, float y, float z);
        
        float _w, _x, _y, _z;
    };

    struct Plane {
        Plane();
        Plane(float x, float y, float z, float p);

        float x, y, z, p;
    };
    
} }

#endif
