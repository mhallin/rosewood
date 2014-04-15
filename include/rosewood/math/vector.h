#ifndef __ROSEWOOD_MATH_VECTOR3_H__
#define __ROSEWOOD_MATH_VECTOR3_H__

#include <math.h>

#include "math_types.h"

namespace rosewood { namespace math {

    // Vector2 functions
    inline         Vector2::Vector2()                   : x(0), y(0) { }
    inline         Vector2::Vector2(float x_, float y_) : x(x_), y(y_) { }

    // Vector3 functions
#ifdef __SSE__
    inline Vector3::Vector3() : m128(_mm_setzero_ps()) { }
    inline Vector3::Vector3(float x, float y, float z) : m128(_mm_set_ps(0, z, y, x)) { }
    inline Vector3::Vector3(const Vector3 &v) : m128(v.m128) { }
    inline Vector3::Vector3(const Vector4 &v) : m128(v.m128) {
        if (fabs(v.w) > 0.01f) {
            auto ws = _mm_set_ps1(v.w);
            m128 = _mm_div_ps(m128, ws);
        }
    }
    inline Vector3::Vector3(__m128 v) : m128(v) { }

    inline float dot(const Vector3 &lhs, const Vector3 &rhs) {
        // prod = x1x2, y1y2, z1z2, (w1w2 = 0)
        auto prod = _mm_mul_ps(lhs.m128, rhs.m128);
        auto t1 = _mm_hadd_ps(prod, prod);
        auto t2 = _mm_hadd_ps(t1, t1);
        float r[4];
        _mm_store_ss(r, t2);
        return r[0];
    }
#else
    inline Vector3::Vector3() : _x(0), _y(0), _z(0), _w(0) { }
    inline Vector3::Vector3(float x, float y, float z) : _x(x), _y(y), _z(z), _w(0) { }
    inline Vector3::Vector3(const Vector4 &v) {
        *this = Vector3(v.x, v.y, v.z);
        if (fabs(v.w) > 0.01f) {
            _x /= v.w;
            _y /= v.y;
            _z /= v.z;
        }
    }

    inline float dot(const Vector3 &lhs, const Vector3 &rhs) {
        // prod = x1x2, y1y2, z1z2, (w1w2 = 0)
        return lhs.x()*rhs.x() + lhs.y()*rhs.y() + lhs.z()*rhs.z();
    }
#endif

    inline float length2(const Vector3 &v) { return dot(v, v); }
    inline float length(const Vector3 &v) { return sqrtf(length2(v)); }
    inline Vector3 cross(const Vector3 &lhs, const Vector3 &rhs) {
        return Vector3(lhs.y()*rhs.z() - lhs.z()*rhs.y(),
                       lhs.z()*rhs.x() - lhs.x()*rhs.z(),
                       lhs.x()*rhs.y() - lhs.y()*rhs.x());
    }

#ifdef __SSE__
    inline Vector3 emult(const Vector3 &lhs, const Vector3 &rhs) {
        return Vector3(_mm_mul_ps(lhs.m128, rhs.m128));
    }
#else
    inline Vector3 emult(const Vector3 &lhs, const Vector3 &rhs) {
        return Vector3(lhs.x()*rhs.x(), lhs.y()*rhs.y(), lhs.z()*rhs.z());
    }
#endif
    inline const float *ptr(const Vector3 &v) { return reinterpret_cast<const float*>(&v); }
    inline float *ptr(Vector3 &v) { return reinterpret_cast<float*>(&v); }

    inline float get(const Vector3 &v, size_t i) { return ptr(v)[i]; }
    inline void set(Vector3 &v, size_t i, float f) { ptr(v)[i] = f; }

    void construct_plane(const Vector3 n, Vector3 *out_p1, Vector3 *out_p2);

#ifdef __SSE__
    inline Vector3 &Vector3::operator=(const Vector3 &rhs) {
        m128 = rhs.m128;
        return *this;
    }
#endif

#ifdef __SSE__
    inline Vector3 &operator+=(Vector3 &lhs, const Vector3 &rhs) {
        lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
        return lhs;
    }

    inline Vector3 &operator-=(Vector3 &lhs, const Vector3 &rhs) {
        lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
        return lhs;
    }

    inline Vector3 &operator*=(Vector3 &lhs, float rhs) {
        auto rhs_v = _mm_set_ps1(rhs);
        lhs.m128 = _mm_mul_ps(lhs.m128, rhs_v);
        return lhs;
    }

    inline Vector3 &operator/=(Vector3 &lhs, float rhs) {
        auto rhs_v = _mm_set_ps1(rhs);
        lhs.m128 = _mm_div_ps(lhs.m128, rhs_v);
        return lhs;
    }
#else
    inline Vector3 &operator+=(Vector3 &lhs, const Vector3 &rhs) {
        lhs.set_x(lhs.x() + rhs.x());
        lhs.set_y(lhs.y() + rhs.y());
        lhs.set_z(lhs.z() + rhs.z());
        return lhs;
    }

    inline Vector3 &operator-=(Vector3 &lhs, const Vector3 &rhs) {
        lhs.set_x(lhs.x() - rhs.x());
        lhs.set_y(lhs.y() - rhs.y());
        lhs.set_z(lhs.z() - rhs.z());
        return lhs;
    }

    inline Vector3 &operator*=(Vector3 &lhs, float rhs) {
        lhs.set_x(lhs.x() * rhs);
        lhs.set_y(lhs.y() * rhs);
        lhs.set_z(lhs.z() * rhs);
        return lhs;
    }

    inline Vector3 &operator/=(Vector3 &lhs, float rhs) {
        lhs.set_x(lhs.x() / rhs);
        lhs.set_y(lhs.y() / rhs);
        lhs.set_z(lhs.z() / rhs);
        return lhs;
    }
#endif

    inline Vector3 operator-(const Vector3 &v) { return Vector3(-v.x(), -v.y(), -v.z()); }
    inline Vector3 operator+(Vector3 lhs, const Vector3 &rhs) { return lhs += rhs; }
    inline Vector3 operator-(Vector3 lhs, const Vector3 &rhs) { return lhs -= rhs; }
    inline Vector3 operator*(float lhs, Vector3 rhs) { return rhs *= lhs; }
    inline Vector3 operator*(Vector3 lhs, float rhs) { return lhs *= rhs; }
    inline Vector3 operator/(Vector3 lhs, float rhs) { return lhs /= rhs; }
#ifdef __SSE__
    inline Vector3 operator/(float lhs, const Vector3 &rhs) {
        auto lhs_v = _mm_set_ps1(lhs);
        return Vector3(_mm_div_ps(lhs_v, rhs.m128));
    }
#else
    inline Vector3 operator/(float lhs, const Vector3 &rhs) {
        return Vector3(lhs / rhs.x(), lhs / rhs.y(), lhs / rhs.z());
    }
#endif

    inline Vector3 normalized(const Vector3 &v) { return v/length(v); }

    inline bool operator==(const Vector3 &lhs, const Vector3 &rhs) {
        return (lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z());
    }

    inline bool operator!=(const Vector3 &lhs, const Vector3 &rhs) {
        return !(lhs == rhs);
    }

    // Vector4 functions
#ifdef __SSE__
    inline Vector4::Vector4() : m128(_mm_set_ps1(0)) { };
    inline Vector4::Vector4(float x, float y, float z, float w) : m128(_mm_set_ps(w, z, y, x)) { };
    inline Vector4::Vector4(const Vector4 &v) : m128(v.m128) { }
    inline Vector4::Vector4(const Vector3 &v, float w) : m128(v.m128) {
        this->w = w;
    }
    inline Vector4::Vector4(__m128 v) : m128(v) { }

    inline float dot(const Vector4 &lhs, const Vector4 &rhs) {
        auto prod = _mm_mul_ps(lhs.m128, rhs.m128);
        auto t1 = _mm_hadd_ps(prod, prod);
        auto t2 = _mm_hadd_ps(t1, t1);
        float r[4];
        _mm_store_ss(r, t2);
        return r[0];
    }
#else
    inline Vector4::Vector4() : x(0), y(0), z(0), w(0) { };
    inline Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { };
    inline Vector4::Vector4(const Vector3 &v, float w) : x(v.x()), y(v.y()), z(v.z()), w(w) { }

    inline float dot(const Vector4 &lhs, const Vector4 &rhs) {
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
    }
#endif
    inline float length2(const Vector4 &v) { return dot(v, v); }
    inline float length(const Vector4 &v) { return sqrtf(length2(v)); }

    inline const float *ptr(const Vector4 &v) { return &v.x; }
    inline float *ptr(Vector4 &v) { return &v.x; }

    inline float get(const Vector4 &v, size_t i) { return ptr(v)[i]; }
    inline void set(Vector4 &v, size_t i, float f) { ptr(v)[i] = f; }

#ifdef __SSE__
    inline Vector4 &Vector4::operator=(const Vector4 &rhs) {
        m128 = rhs.m128;
        return *this;
    }
#endif

#ifdef __SSE__
    inline Vector4 &operator+=(Vector4 &lhs, const Vector4 &rhs) {
        lhs.m128 = _mm_add_ps(lhs.m128, rhs.m128);
        return lhs;
    }

    inline Vector4 &operator-=(Vector4 &lhs, const Vector4 &rhs) {
        lhs.m128 = _mm_sub_ps(lhs.m128, rhs.m128);
        return lhs;
    }

    inline Vector4 &operator*=(Vector4 &lhs, float rhs) {
        auto rhs_v = _mm_set_ps1(rhs);
        lhs.m128 = _mm_mul_ps(lhs.m128, rhs_v);
        return lhs;
    }

    inline Vector4 &operator/=(Vector4 &lhs, float rhs) {
        auto rhs_v = _mm_set_ps1(rhs);
        lhs.m128 = _mm_div_ps(lhs.m128, rhs_v);
        return lhs;
    }
#else
    inline Vector4 &operator+=(Vector4 &lhs, const Vector4 &rhs) {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        lhs.w += rhs.w;
        return lhs;
    }

    inline Vector4 &operator-=(Vector4 &lhs, const Vector4 &rhs) {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        lhs.w -= rhs.w;
        return lhs;
    }

    inline Vector4 &operator*=(Vector4 &lhs, float rhs) {
        lhs.x *= rhs;
        lhs.y *= rhs;
        lhs.z *= rhs;
        lhs.w *= rhs;
        return lhs;
    }

    inline Vector4 &operator/=(Vector4 &lhs, float rhs) {
        lhs.x /= rhs;
        lhs.y /= rhs;
        lhs.z /= rhs;
        lhs.w /= rhs;
        return lhs;
    }
#endif

    inline Vector4 operator-(const Vector4 &v) {
        return Vector4(-v.x, -v.y, -v.z, -v.w);
    }
    inline Vector4 operator+(Vector4 lhs, const Vector4 &rhs) { return lhs += rhs; }
    inline Vector4 operator-(Vector4 lhs, const Vector4 &rhs) { return lhs -= rhs; }
    inline Vector4 operator*(Vector4 lhs, float rhs) { return lhs *= rhs; }
    inline Vector4 operator/(Vector4 lhs, float rhs) { return lhs /= rhs; }

#ifdef __SSE__
    inline Vector4 operator/(float lhs, const Vector4 &rhs) {
        auto lhs_v = _mm_set_ps1(lhs);
        return Vector4(_mm_div_ps(lhs_v, rhs.m128));
    }
#else
    inline Vector4 operator/(float lhs, const Vector4 &rhs) {
        return Vector4(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
    }
#endif

    inline Vector4 normalized(const Vector4 &v) { return v/length(v); }

    inline bool operator==(const Vector4 &lhs, const Vector4 &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }
    inline bool operator!=(const Vector4 &lhs, const Vector4 &rhs) {
        return !(lhs == rhs);
    }

} }

#endif
