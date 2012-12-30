#include "rosewood/math/matrix3.h"

#include "rosewood/math/vector.h"

using rosewood::math::Matrix3;
using rosewood::math::Vector3;

Matrix3::Matrix3() {
    std::fill(begin(_m), end(_m), 0);
}

Matrix3::Matrix3(float m11, float m12, float m13,
                 float m21, float m22, float m23,
                 float m31, float m32, float m33) {
    size_t i = 0;

    // Column 1
    _m[i++] = m11; _m[i++] = m21; _m[i++] = m31;

    // Column 2
    _m[i++] = m12; _m[i++] = m22; _m[i++] = m32;

    // Column 3
    _m[i++] = m13; _m[i++] = m23; _m[i++] = m33;
}

float rosewood::math::get(const Matrix3 &m, size_t row, size_t col) {
    return m._m[col*4 + row];
}

void rosewood::math::set(Matrix3 &m, size_t row, size_t col, float v) {
    m._m[col*4 + row] = v;
}

Matrix3 rosewood::math::transposed(const Matrix3 &m) {
    return Matrix3(m._m[0], m._m[1], m._m[2],
                   m._m[3], m._m[4], m._m[5],
                   m._m[6], m._m[7], m._m[8]);
}

bool rosewood::math::operator==(const rosewood::math::Matrix3 &lhs,
                                 const rosewood::math::Matrix3 &rhs) {
    return std::equal(begin(lhs), end(lhs), begin(rhs));
}

bool rosewood::math::operator!=(const rosewood::math::Matrix3 &lhs,
                                 const rosewood::math::Matrix3 &rhs) {
    return !(lhs == rhs);
}

Vector3 rosewood::math::operator*(const Matrix3 &lhs, const Vector3 &rhs) {
    Vector3 result;

    float * __restrict r = ptr(result);
    const float * __restrict a = ptr(lhs), * __restrict b = ptr(rhs);

    r[0] = a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
    r[1] = a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
    r[2] = a[2]*b[0] + a[5]*b[1] + a[8]*b[2];

    return result;
}
