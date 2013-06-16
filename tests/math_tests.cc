#include <gtest/gtest.h>

#include "rosewood/math/math_types.h"
#include "rosewood/math/vector.h"
#include "rosewood/math/matrix4.h"

using namespace rosewood::math;

TEST(MathTests, Vector3Initialization) {
    Vector3 v(1, 2, 3);

    EXPECT_EQ(1, v.x());
    EXPECT_EQ(2, v.y());
    EXPECT_EQ(3, v.z());
}

TEST(MathTests, Vector4Initialization) {
    Vector4 v(1, 2, 3, 4);

    EXPECT_EQ(1, v.x);
    EXPECT_EQ(2, v.y);
    EXPECT_EQ(3, v.z);
    EXPECT_EQ(4, v.w);
}

TEST(MathTests, Vector3FloatMult) {
    auto v1 = Vector3(1, 2, 3) * 3;

    EXPECT_EQ(3, v1.x());
    EXPECT_EQ(6, v1.y());
    EXPECT_EQ(9, v1.z());
}

TEST(MathTests, Vector3Vector3Add) {
    auto v1 = Vector3(1, 2, 3) + Vector3(4, 1, 3);

    EXPECT_EQ(5, v1.x());
    EXPECT_EQ(3, v1.y());
    EXPECT_EQ(6, v1.z());
}

TEST(MathTests, Matrix4Vector3Scale) {
    auto v1 = make_scale4(Vector3(2, 3, 4)) * Vector3(4, 5, 6);

    EXPECT_EQ(8, v1.x());
    EXPECT_EQ(15, v1.y());
    EXPECT_EQ(24, v1.z());
}

TEST(MathTests, Matrix4Vector4Scale) {
    auto v1 = make_scale4(Vector3(2, 3, 4)) * Vector4(4, 5, 6, 1);

    EXPECT_EQ(8, v1.x);
    EXPECT_EQ(15, v1.y);
    EXPECT_EQ(24, v1.z);
    EXPECT_EQ(1, v1.w);
}

TEST(MathTests, Matrix4Vector3Translate) {
    auto v1 = make_translation4(Vector3(2, 3, 4)) * Vector3(4, 5, 6);

    EXPECT_EQ(6, v1.x());
    EXPECT_EQ(8, v1.y());
    EXPECT_EQ(10, v1.z());
}

TEST(MathTests, Matrix4Vector4Translate) {
    auto v1 = make_translation4(Vector3(2, 3, 4)) * Vector4(4, 5, 6, 1);

    EXPECT_EQ(6, v1.x);
    EXPECT_EQ(8, v1.y);
    EXPECT_EQ(10, v1.z);
    EXPECT_EQ(1, v1.w);
}
