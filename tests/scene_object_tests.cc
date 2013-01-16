#include <gtest/gtest.h>

#include "rosewood/math/vector.h"
#include "rosewood/math/quaternion.h"

#include "rosewood/core/transform.h"

using namespace rosewood::math;
using namespace rosewood::core;

TEST(SceneObjectTest, IdentityInitialization) {
    Transform transform(Entity{nullptr, 0});
    
    EXPECT_EQ(nullptr, transform.parent());

    EXPECT_EQ(Vector3(0, 0, 0), transform.local_position());
    EXPECT_EQ(Vector3(0, 0, 0), transform.world_position());
    
    EXPECT_EQ(Vector3(1, 1, 1), transform.local_scale());
    
    EXPECT_EQ(quaternion_identity(), transform.local_rotation());
    EXPECT_EQ(quaternion_identity(), transform.world_rotation());
}
