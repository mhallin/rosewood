#include "rosewood/math/matrix4.h"

#include "rosewood/math/vector.h"
#include "rosewood/math/trig.h"
#include "rosewood/math/quaternion.h"

using rosewood::math::Matrix4;
using rosewood::math::Matrix3;
using rosewood::math::Quaternion;
using rosewood::math::Vector4;
using rosewood::math::Vector3;

Matrix4::Matrix4() {
    std::fill(std::begin(_m), std::end(_m), 0);
}

Matrix4::Matrix4(float m11, float m12, float m13, float m14,
                 float m21, float m22, float m23, float m24,
                 float m31, float m32, float m33, float m34,
                 float m41, float m42, float m43, float m44) {
    size_t i = 0;

    // Column 1
    _m[i++] = m11; _m[i++] = m21; _m[i++] = m31; _m[i++] = m41;

    // Column 2
    _m[i++] = m12; _m[i++] = m22; _m[i++] = m32; _m[i++] = m42;

    // Column 3
    _m[i++] = m13; _m[i++] = m23; _m[i++] = m33; _m[i++] = m43;

    // Column 4
    _m[i++] = m14; _m[i++] = m24; _m[i++] = m34; _m[i++] = m44;
}

float rosewood::math::get(const Matrix4 &m, size_t row, size_t col) {
    return m._m[col*4 + row];
}

void rosewood::math::set(Matrix4 &m, size_t row, size_t col, float v) {
    m._m[col*4 + row] = v;
}

Matrix4 rosewood::math::transposed(const Matrix4 &m) {
    return Matrix4(m._m[ 0], m._m[ 1], m._m[ 2], m._m[ 3],
                   m._m[ 4], m._m[ 5], m._m[ 6], m._m[ 7],
                   m._m[ 8], m._m[ 9], m._m[10], m._m[11],
                   m._m[12], m._m[13], m._m[14], m._m[15]);
}

Matrix3 rosewood::math::mat3(const Matrix4 &m) {
    return Matrix3(get(m, 0, 0), get(m, 0, 1), get(m, 0, 2),
                   get(m, 1, 0), get(m, 1, 1), get(m, 1, 2),
                   get(m, 2, 0), get(m, 2, 1), get(m, 2, 2));
}

void rosewood::math::apply_translate_by(Matrix4 &m, Vector3 v) {
    m *= make_translation4(v);
}

void rosewood::math::apply_rotate_by(Matrix4 &m, Quaternion q) {
    m *= make_rotation4(q);
}

void rosewood::math::apply_scale_by(Matrix4 &m, Vector3 v) {
    m *= make_scale4(v);
}

Matrix4 rosewood::math::make_identity4() {
    return Matrix4(1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1);
}

Matrix4 rosewood::math::make_hand_shift4() {
    return Matrix4(1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, -1, 0,
                   0, 0, 0, 1);
}

Matrix4 rosewood::math::make_perspective4(float fovy,
                                          float aspect,
                                          float z_near,
                                          float z_far) {
    float f = cotf(fovy/2);

    return Matrix4(f/aspect, 0,                             0,                                 0,
                   0,        f,                             0,                                 0,
                   0,        0, (z_far+z_near)/(z_near-z_far), (2*z_far*z_near)/(z_near - z_far),
                   0,        0,                            -1,                                 0);
}

Matrix4 rosewood::math::make_inverse_perspective4(float fovy,
                                                  float aspect,
                                                  float z_near,
                                                  float z_far) {
    float f = cotf(fovy/2);

    return Matrix4(aspect/f,   0,                               0,                               0,
                   0,        1/f,                               0,                               0,
                   0,          0,                               0,                              -1,
                   0,          0, -(z_near-z_far)/(2*z_far*z_near), -(z_far+z_near)/(2*z_far*z_near));
}

Matrix4 rosewood::math::make_translation4(Vector3 v) {
    return Matrix4(1, 0, 0, v.x(),
                   0, 1, 0, v.y(),
                   0, 0, 1, v.z(),
                   0, 0, 0,   1);
}

Matrix4 rosewood::math::make_rotation4(Quaternion q) {
    return mat4(q);
}

Matrix4 rosewood::math::make_scale4(Vector3 v) {
    return Matrix4(v.x(),   0,     0,   0,
                     0, v.y(),     0,   0,
                     0,     0, v.z(),   0,
                     0,     0,     0,   1);
}

Matrix4 &rosewood::math::operator*=(Matrix4 &lhs, const Matrix4 &rhs) {
    lhs = lhs * rhs;
    return lhs;
}

Matrix4 rosewood::math::operator*(const Matrix4 &lhs, const Matrix4 &rhs) {
    Matrix4 result;

    float * __restrict r = ptr(result);
    const float * __restrict a = ptr(lhs), * __restrict b = ptr(rhs);

#ifdef _ARM_ARCH_7
    asm volatile
    (
     // Load LHS to the Q0-Q3 (D0-D7) registers and RHS to the Q4-Q7 (D8-D11) registers.
     "vldm %0, { q0-q3 } \n"
     "vldm %1, { q4-q7 } \n"

     // Calculate first column of result matrix: Multiply first column in LHS with first row of RHS.
     // The rows are accessed by scalars in D8 & D9
     "vmul.f32 q8, q0, d8[0] \n"
     "vmla.f32 q8, q1, d8[1] \n"
     "vmla.f32 q8, q2, d9[0] \n"
     "vmla.f32 q8, q3, d9[1] \n"

     // Second column of result matrix
     "vmul.f32 q9, q0, d10[0] \n"
     "vmla.f32 q9, q1, d10[1] \n"
     "vmla.f32 q9, q2, d11[0] \n"
     "vmla.f32 q9, q3, d11[1] \n"

     // Third column of result matrix
     "vmul.f32 q10, q0, d12[0] \n"
     "vmla.f32 q10, q1, d12[1] \n"
     "vmla.f32 q10, q2, d13[0] \n"
     "vmla.f32 q10, q3, d13[1] \n"

     // Fourth column of result matrix
     "vmul.f32 q11, q0, d14[0] \n"
     "vmla.f32 q11, q1, d14[1] \n"
     "vmla.f32 q11, q2, d15[0] \n"
     "vmla.f32 q11, q3, d15[1] \n"

     // Store Q8-Q11 at the result pointer
     "vstm %2, { q8-q11 } \n"
     :
     : "r"(a), "r"(b), "r"(r)
     : "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11"
    );
#else
    r[ 0] = a[ 0]*b[ 0] + a[ 4]*b[ 1] + a[ 8]*b[ 2] + a[12]*b[ 3];
    r[ 1] = a[ 1]*b[ 0] + a[ 5]*b[ 1] + a[ 9]*b[ 2] + a[13]*b[ 3];
    r[ 2] = a[ 2]*b[ 0] + a[ 6]*b[ 1] + a[10]*b[ 2] + a[14]*b[ 3];
    r[ 3] = a[ 3]*b[ 0] + a[ 7]*b[ 1] + a[11]*b[ 2] + a[15]*b[ 3];
    r[ 4] = a[ 0]*b[ 4] + a[ 4]*b[ 5] + a[ 8]*b[ 6] + a[12]*b[ 7];
    r[ 5] = a[ 1]*b[ 4] + a[ 5]*b[ 5] + a[ 9]*b[ 6] + a[13]*b[ 7];
    r[ 6] = a[ 2]*b[ 4] + a[ 6]*b[ 5] + a[10]*b[ 6] + a[14]*b[ 7];
    r[ 7] = a[ 3]*b[ 4] + a[ 7]*b[ 5] + a[11]*b[ 6] + a[15]*b[ 7];
    r[ 8] = a[ 0]*b[ 8] + a[ 4]*b[ 9] + a[ 8]*b[10] + a[12]*b[11];
    r[ 9] = a[ 1]*b[ 8] + a[ 5]*b[ 9] + a[ 9]*b[10] + a[13]*b[11];
    r[10] = a[ 2]*b[ 8] + a[ 6]*b[ 9] + a[10]*b[10] + a[14]*b[11];
    r[11] = a[ 3]*b[ 8] + a[ 7]*b[ 9] + a[11]*b[10] + a[15]*b[11];
    r[12] = a[ 0]*b[12] + a[ 4]*b[13] + a[ 8]*b[14] + a[12]*b[15];
    r[13] = a[ 1]*b[12] + a[ 5]*b[13] + a[ 9]*b[14] + a[13]*b[15];
    r[14] = a[ 2]*b[12] + a[ 6]*b[13] + a[10]*b[14] + a[14]*b[15];
    r[15] = a[ 3]*b[12] + a[ 7]*b[13] + a[11]*b[14] + a[15]*b[15];
#endif

    return result;
}

Vector3 rosewood::math::operator*(const Matrix4 &lhs, const Vector3 &rhs) {
    Vector3 result;
    float w;

    float * __restrict r = ptr(result);
    const float * __restrict a = ptr(lhs), * __restrict b = ptr(rhs);

    w     =  a[ 3]*b[ 0] + a[ 7]*b[ 1] + a[11]*b[ 2] + a[15];
    r[ 0] = (a[ 0]*b[ 0] + a[ 4]*b[ 1] + a[ 8]*b[ 2] + a[12]) / w;
    r[ 1] = (a[ 1]*b[ 0] + a[ 5]*b[ 1] + a[ 9]*b[ 2] + a[13]) / w;
    r[ 2] = (a[ 2]*b[ 0] + a[ 6]*b[ 1] + a[10]*b[ 2] + a[14]) / w;

    return result;
}

Vector4 rosewood::math::operator*(const Matrix4 &lhs, const Vector4 &rhs) {
    Vector4 result;

    float * __restrict r = ptr(result);
    const float * __restrict a = ptr(lhs), * __restrict b = ptr(rhs);

#ifdef _ARM_ARCH_7
    asm volatile
    (
     // Load LHS to the Q0-Q3 (D0-D7) registers and RHS to the Q4 (D8-D9) register.
     "vldm %0, { q0-q3 } \n"
     "vldm %1, { q4 } \n"

     // Calculate first column of result matrix: Multiply first column in LHS with RHS.
     "vmul.f32 q5, q0, d8[0] \n"
     "vmla.f32 q5, q1, d8[1] \n"
     "vmla.f32 q5, q2, d9[0] \n"
     "vmla.f32 q5, q3, d9[1] \n"

     // Store Q8-Q11 at the result pointer
     "vstm %2, { q5 } \n"
     :
     : "r"(a), "r"(b), "r"(r)
     : "memory", "q0", "q1", "q2", "q3", "q4", "q5"
     );
#else
    r[ 0] = a[ 0]*b[ 0] + a[ 4]*b[ 1] + a[ 8]*b[ 2] + a[12]*b[ 3];
    r[ 1] = a[ 1]*b[ 0] + a[ 5]*b[ 1] + a[ 9]*b[ 2] + a[13]*b[ 3];
    r[ 2] = a[ 2]*b[ 0] + a[ 6]*b[ 1] + a[10]*b[ 2] + a[14]*b[ 3];
    r[ 3] = a[ 3]*b[ 0] + a[ 7]*b[ 1] + a[11]*b[ 2] + a[15]*b[ 3];
#endif

    return result;
}

bool rosewood::math::operator==(const rosewood::math::Matrix4 &lhs,
                                 const rosewood::math::Matrix4 &rhs) {
    return std::equal(begin(lhs), end(lhs), begin(rhs));
}

bool rosewood::math::operator!=(const rosewood::math::Matrix4 &lhs,
                                 const rosewood::math::Matrix4 &rhs) {
    return !(lhs == rhs);
}
