#include <gtest/gtest.h>

#include "rosewood/data-structures/stable_vector.h"

using namespace rosewood::data_structures;

TEST(StableVectorTests, ZeroInitialization) {
    StableVector<int> sv;
    
    EXPECT_EQ(0, sv.size());
    EXPECT_TRUE(sv.empty());
}

TEST(StableVectorTests, AddElement) {
    StableVector<int> sv;
    
    sv.push_back(5);
    
    EXPECT_EQ(1, sv.size());
    EXPECT_FALSE(sv.empty());
    
    EXPECT_EQ(5, static_cast<const StableVector<int>&>(sv)[0]);
}

TEST(StableVectorTests, SetElement) {
    StableVector<int> sv;
    
    sv.push_back(5);
    sv[0] = 3;
    
    EXPECT_EQ(3, sv[0]);
}

TEST(StableVectorTests, ResizeGrow) {
    StableVector<int> sv;
    
    sv.resize(5);
    
    EXPECT_EQ(5, sv.size());
}

TEST(StableVectorTests, ResizeShrink) {
    StableVector<int> sv;
    
    for (int i = 0; i < 10; ++i) sv.push_back(i);
    
    sv.resize(5);
    
    EXPECT_EQ(5, sv.size());
}

TEST(StableVectorTests, ResizeGrowFill) {
    StableVector<int> sv;
    
    sv.resize(3, 23);
    
    ASSERT_EQ(3, sv.size());
    EXPECT_EQ(23, sv[0]);
    EXPECT_EQ(23, sv[1]);
    EXPECT_EQ(23, sv[2]);
}

TEST(StableVectorTests, SmallSliceSize) {
    StableVector<int> sv(3);
    
    for (int i = 0; i < 3; ++i) sv.push_back(i);
    
    int *i2 = &sv[2];
    
    for (int i = 0; i < 10; ++i) sv.push_back(i);
    
    EXPECT_EQ(i2, &sv[2]);
    
    sv.resize(3);
    
    EXPECT_EQ(i2, &sv[2]);
}
