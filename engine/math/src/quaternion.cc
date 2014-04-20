#include "rosewood/math/quaternion.h"

#include <math.h>

#include "rosewood/math/vector.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/trig.h"

using rosewood::math::Quaternion;
using rosewood::math::Vector3;
using rosewood::math::Matrix4;

Quaternion rosewood::math::quaternion_from_axis_angle(const Vector3 axis, float angle) {
    auto vn = normalized(axis);
    auto hangle = angle / 2;
    auto sa = sinf(hangle);

    auto w = cosf(hangle);
    auto x = vn.x() * sa;
    auto y = vn.y() * sa;
    auto z = vn.z() * sa;

    return Quaternion(w, x, y, z);
}

Quaternion rosewood::math::quaternion_from_euler_angles(float x, float y, float z) {
    auto c1 = cosf(y/2), s1 = sinf(y/2);
    auto c2 = cosf(z/2), s2 = sinf(z/2);
    auto c3 = cosf(x/2), s3 = sinf(x/2);

    return Quaternion(c1*c2*c3 - s1*s2*s3,
                      s1*s2*c3 + c1*c2*s3,
                      s1*c2*c3 + c1*s2*s3,
                      c1*s2*c3 - s1*c2*s3);
}

Quaternion rosewood::math::quaternion_from_euler_angles(Vector3 euler_angles) {
    return quaternion_from_euler_angles(
        euler_angles.x(), euler_angles.y(), euler_angles.z());
}

Matrix4 rosewood::math::mat4(const Quaternion q) {
    auto w2 = q._w*q._w, x2 = q._x*q._x, y2 = q._y*q._y, z2 = q._z*q._z;
    auto wx = q._w*q._x, wy = q._w*q._y, wz = q._w*q._z;
    auto xy = q._x*q._y, xz = q._x*q._z;
    auto yz = q._y*q._z;

    return Matrix4(w2 + x2 - y2 - z2,       2*xy - 2*wz,       2*xz + 2*wy, 0,
                         2*xy + 2*wz, w2 - x2 + y2 - z2,       2*yz - 2*wx, 0,
                         2*xz - 2*wy,       2*yz + 2*wx, w2 - x2 - y2 + z2, 0,
                                   0,                 0,                 0, 1);
}

float rosewood::math::norm(const Quaternion q) {
    return sqrtf(q._x*q._x + q._y*q._y + q._z*q._z + q._w*q._w);
}

float rosewood::math::dot(const Quaternion lhs, const Quaternion rhs) {
    return lhs._x*rhs._x + lhs._y*rhs._y + lhs._z*rhs._z + lhs._w*rhs._w;
}

Quaternion rosewood::math::operator*(const Quaternion lhs, const Quaternion rhs) {
    return Quaternion(lhs._w*rhs._w - lhs._x*rhs._x - lhs._y*rhs._y - lhs._z*rhs._z,
                      lhs._w*rhs._x + lhs._x*rhs._w + lhs._y*rhs._z - lhs._z*rhs._y,
                      lhs._w*rhs._y + lhs._y*rhs._w + lhs._z*rhs._x - lhs._x*rhs._z,
                      lhs._w*rhs._z + lhs._z*rhs._w + lhs._x*rhs._y - lhs._y*rhs._x);
}

Vector3 rosewood::math::operator*(const Quaternion q, const Vector3 v) {
    return mat4(q) * v;
}

float rosewood::math::rotation_around(const Quaternion q, const Vector3 axis) {
    auto n_axis = normalized(axis);
    Vector3 p1;

    construct_plane(n_axis, &p1, nullptr);

    auto transformed = q * p1;
    auto in_plane = transformed - dot(transformed, n_axis) * n_axis;
    return signed_angle_between(p1, in_plane, n_axis);
    // return acosf(dot(p1, in_plane));
}
