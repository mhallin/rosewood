#ifndef __ROSEWOOD_MATH_MATH_TYPES_H__
#define __ROSEWOOD_MATH_MATH_TYPES_H__

#include <array>

#include <TargetConditionals.h>

#if __SSE__
#include <x86intrin.h>
#endif

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

        union {
            float _m[kSize];
#if __SSE__
            struct {
                __m128 _c1, _c2, _c3, _c4;
            };
#endif
        };
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
        Vector4(const Vector4 &v);
        explicit Vector4(const Vector3 &v, float w);

#if __SSE__
        explicit Vector4(__m128 v);
#endif

        Vector4 &operator=(const Vector4 &rhs);

        union {
            struct {
                float x, y, z, w;
            };
#if __SSE__
            __m128 m128;
#endif
        };
    };

    struct Vector3 {
        Vector3();
        Vector3(float x, float y, float z);
        Vector3(const Vector3 &v);
        explicit Vector3(const Vector4 &v);
#if __SSE__
        explicit Vector3(__m128 v);
#endif

        Vector3 &operator=(const Vector3 &rhs);

#if __SSE__
        __m128 m128;

        float x() const { float r[4]; _mm_store_ps(r, m128); return r[0]; }
        float y() const { float r[4]; _mm_store_ps(r, m128); return r[1]; }
        float z() const { float r[4]; _mm_store_ps(r, m128); return r[2]; }

        void set_x(float x) {
            float r[4]; _mm_store_ps(r, m128); r[0] = x; m128 = _mm_load_ps(r);
        }
        void set_y(float y) {
            float r[4]; _mm_store_ps(r, m128); r[1] = y; m128 = _mm_load_ps(r);
        }
        void set_z(float z) {
            float r[4]; _mm_store_ps(r, m128); r[2] = z; m128 = _mm_load_ps(r);
        }
#endif
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
