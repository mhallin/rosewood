#include <gtest/gtest.h>

#include "rosewood/math/vector.h"
#include "rosewood/math/quaternion.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/trig.h"
#include "rosewood/math/math_ostream.h"

#include "rosewood/core/transform.h"

using namespace rosewood::math;
using namespace rosewood::core;

static bool quat_eq(Quaternion q1, Quaternion q2) {
    static const double epsilon = 1e-5;

    return (fabs(q1._w - q2._w) < epsilon && fabs(q1._x - q2._x) < epsilon &&
            fabs(q1._y - q2._y) < epsilon && fabs(q1._z - q2._z) < epsilon);
}

class TransformTests : public ::testing::Test {
protected:
    // construct the following transform tree:
    //
    //        /-- inner -
    // _root -           \-- leaf1
    //        \-- leaf2
    //
    virtual void SetUp() override {
        _root = transform(_entities.create_entity<Transform>());
        _inner = transform(_entities.create_entity<Transform>());
        _leaf1 = transform(_entities.create_entity<Transform>());
        _leaf2 = transform(_entities.create_entity<Transform>());

        _root->add_child(_inner);
        _root->add_child(_leaf2);

        _inner->add_child(_leaf1);
    }

    EntityManager _entities;

    Transform *_root;
    Transform *_inner;
    Transform *_leaf1;
    Transform *_leaf2;
};

TEST_F(TransformTests, IdentityInitialization) {
    Transform transform(nil_entity());

    EXPECT_EQ(nullptr, transform.parent());

    EXPECT_EQ(Vector3(0, 0, 0), transform.local_position());
    EXPECT_EQ(Vector3(0, 0, 0), transform.world_position());

    EXPECT_EQ(Vector3(1, 1, 1), transform.local_scale());

    EXPECT_PRED2(quat_eq, quaternion_identity(), transform.local_rotation());
    EXPECT_PRED2(quat_eq, quaternion_identity(), transform.world_rotation());
}

TEST_F(TransformTests, WorldPosition) {
    _inner->set_local_position(1, 3, 2);
    _leaf1->set_local_position(4, 1, 1);

    EXPECT_EQ(Vector3(5, 4, 3), _leaf1->world_position());

    _inner->set_local_position(0, 2, 1, ChildPositions::KeepWorld);
    EXPECT_EQ(Vector3(0, 2, 1), _inner->local_position());
    EXPECT_EQ(Vector3(5, 4, 3), _leaf1->world_position());
}

TEST_F(TransformTests, WorldRotation) {
    _inner->set_local_axis_angle(0, 1, 0, deg2rad(45));
    _leaf1->set_local_axis_angle(0, 1, 0, deg2rad(45));

    auto y90 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(90));

    EXPECT_PRED2(quat_eq, y90, _leaf1->world_rotation());
}

TEST_F(TransformTests, SetWorldPosition) {
    _inner->set_local_position(1, 3, 2);

    _leaf1->set_world_position(5, 4, 3);
    EXPECT_EQ(Vector3(4, 1, 1), _leaf1->local_position());
}

TEST_F(TransformTests, SetWorldRotation) {
    _inner->set_local_axis_angle(0, 1, 0, deg2rad(45));

    _leaf1->set_world_axis_angle(0, 1, 0, deg2rad(90));
    auto y45 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(45));
    EXPECT_PRED2(quat_eq, y45, _leaf1->local_rotation());
}

TEST_F(TransformTests, WorldToLocalPosition) {
    _inner->set_local_position(1, 3, 2);
    _leaf1->set_local_position(4, 1, 1);

    EXPECT_EQ(Vector3( 0,  0,  0), _leaf1->world_to_local(Vector3(5, 4, 3)));
    EXPECT_EQ(Vector3(-5, -4, -3), _leaf1->world_to_local(Vector3(0, 0, 0)));
}

TEST_F(TransformTests, WorldToLocalRotation) {
    _inner->set_local_axis_angle(0, 1, 0, deg2rad(45));
    _leaf1->set_local_axis_angle(0, 1, 0, deg2rad(45));

    auto y90 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(90));
    auto yNeg90 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(-90));

    EXPECT_PRED2(quat_eq, quaternion_identity(), _leaf1->world_to_local(y90));
    EXPECT_PRED2(quat_eq, yNeg90, _leaf1->world_to_local(quaternion_identity()));
}

TEST_F(TransformTests, LocalToWorldPosition) {
    _inner->set_local_position(1, 3, 2);
    _leaf1->set_local_position(4, 1, 1);

    EXPECT_EQ(Vector3(5, 4, 3), _leaf1->local_to_world(Vector3(0, 0, 0)));
    EXPECT_EQ(Vector3(0, 0, 0), _leaf1->local_to_world(Vector3(-5, -4, -3)));
}

TEST_F(TransformTests, LocalToWorldRotation) {
    _inner->set_local_axis_angle(0, 1, 0, deg2rad(45));
    _leaf1->set_local_axis_angle(0, 1, 0, deg2rad(45));

    auto y90 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(90));
    auto yNeg90 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(-90));

    EXPECT_PRED2(quat_eq, y90, _leaf1->local_to_world(quaternion_identity()));
    EXPECT_PRED2(quat_eq, quaternion_identity(), _leaf1->local_to_world(yNeg90));
}

TEST_F(TransformTests, ConvertPositionFromTransform) {
    _inner->set_local_position(1, 3, 2);
    _leaf1->set_local_position(4, 1, 1);
    _leaf2->set_local_position(-2, 3, 8);

    // Identity conversion when converting from itself
    EXPECT_EQ(Vector3(7, 2, 3), _leaf1->convert_from(Vector3(7, 2, 3), _leaf1));

    // Local position when converting from parent
    EXPECT_EQ(Vector3(0, 0, 0), _leaf1->convert_from(_leaf1->local_position(), _inner));

    // Leaf 1 has world pos (5, 4, 3)
    // Leaf 2 has world pos (-2, 3, 8)
    EXPECT_EQ(Vector3(7, 1, -5), _leaf2->convert_from(Vector3(0, 0, 0), _leaf1));
    EXPECT_EQ(Vector3(-7, -1, 5), _leaf1->convert_from(Vector3(0, 0, 0), _leaf2));
}

TEST_F(TransformTests, ConvertRotationFromTransform) {
    _inner->set_local_axis_angle(0, 1, 0, deg2rad(45));
    _leaf1->set_local_axis_angle(0, 1, 0, deg2rad(45));
    _leaf2->set_local_axis_angle(0, 1, 0, deg2rad(45));

    auto y45 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(45));
    auto yNeg45 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(-45));

    // Identity conversion when converting from itself
    EXPECT_PRED2(quat_eq, y45, _leaf1->convert_from(y45, _leaf1));

    // Local rotation when converting from parent
    EXPECT_PRED2(quat_eq, quaternion_identity(), _leaf1->convert_from(_leaf1->local_rotation(), _inner));

    // Leaf 1 has world rotation (0, 1, 0) / 90
    // Leaf 1 has world rotation (0, 1, 0) / 45
    EXPECT_PRED2(quat_eq, y45, _leaf2->convert_from(quaternion_identity(), _leaf1));
    EXPECT_PRED2(quat_eq, yNeg45, _leaf1->convert_from(quaternion_identity(), _leaf2));
}

TEST_F(TransformTests, ConvertPositionToTransform) {
    _inner->set_local_position(1, 3, 2);
    _leaf1->set_local_position(4, 1, 1);
    _leaf2->set_local_position(-2, 3, 8);

    // Identity conversion when converting to itself
    EXPECT_EQ(Vector3(7, 2, 3), _leaf1->convert_to(Vector3(7, 2, 3), _leaf1));

    // Local position when converting from parent to leaf
    EXPECT_EQ(Vector3(0, 0, 0), _inner->convert_to(_leaf1->local_position(), _leaf1));

    // Leaf 1 has world pos (5, 4, 3)
    // Leaf 2 has world pos (-2, 3, 8)
    EXPECT_EQ(Vector3(7, 1, -5), _leaf1->convert_to(Vector3(0, 0, 0), _leaf2));
    EXPECT_EQ(Vector3(-7, -1, 5), _leaf2->convert_to(Vector3(0, 0, 0), _leaf1));
}

TEST_F(TransformTests, ConvertRotationToTransform) {
    _inner->set_local_axis_angle(0, 1, 0, deg2rad(45));
    _leaf1->set_local_axis_angle(0, 1, 0, deg2rad(45));
    _leaf2->set_local_axis_angle(0, 1, 0, deg2rad(45));

    auto y45 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(45));
    auto yNeg45 = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(-45));

    // Identity conversion when converting to itself
    EXPECT_PRED2(quat_eq, y45, _leaf1->convert_to(y45, _leaf1));

    // Local rotation when converting from parent to leaf
    EXPECT_PRED2(quat_eq, quaternion_identity(), _inner->convert_to(_leaf1->local_rotation(), _leaf1));

    // Leaf 1 has world rotation (0, 1, 0) / 90
    // Leaf 1 has world rotation (0, 1, 0) / 45
    EXPECT_PRED2(quat_eq, y45, _leaf1->convert_to(quaternion_identity(), _leaf2));
    EXPECT_PRED2(quat_eq, yNeg45, _leaf2->convert_to(quaternion_identity(), _leaf1));
}
