#include <gtest/gtest.h>

#include "rosewood/math/vector.h"
#include "rosewood/math/quaternion.h"
#include "rosewood/math/matrix4.h"

#include "rosewood/core/transform.h"

using namespace rosewood::math;
using namespace rosewood::core;

TEST(TransformTests, IdentityInitialization) {
    Transform transform(Entity{nullptr, 0});
    
    EXPECT_EQ(nullptr, transform.parent());

    EXPECT_EQ(Vector3(0, 0, 0), transform.local_position());
    EXPECT_EQ(Vector3(0, 0, 0), transform.world_position());
    
    EXPECT_EQ(Vector3(1, 1, 1), transform.local_scale());
    
    EXPECT_EQ(quaternion_identity(), transform.local_rotation());
    EXPECT_EQ(quaternion_identity(), transform.world_rotation());
}

TEST(TransformTests, ConstructTransformMatrices) {
    Transform transform(Entity{nullptr, 0});
    
    transform.set_local_position(1, 2, 3);
    EXPECT_EQ(Vector3(1, 2, 3), transform.local_transform() * Vector3(0, 0, 0));
    
    transform.set_local_position(4, 5, 6);
    EXPECT_EQ(Vector3(4, 5, 6), transform.world_transform() * Vector3(0, 0, 0));
}

TEST(TransformTests, TransformsInEntityManager) {
    EntityManager entities;
    
    auto e1 = entities.create_entity<Transform>();
    auto e2 = entities.create_entity<Transform>();
    
    auto t1 = e1.component<Transform>();
    auto t2 = e2.component<Transform>();
    
    t2->set_local_position(1, 2, 3);
    t1->set_local_position(4, 5, 6);
    
    EXPECT_EQ(Vector3(1, 2, 3), t2->world_transform() * Vector3(0, 0, 0));
    EXPECT_EQ(Vector3(4, 5, 6), t1->world_transform() * Vector3(0, 0, 0));
}
