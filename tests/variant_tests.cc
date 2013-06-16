#include <gtest/gtest.h>

#include "rosewood/data-structures/variant.h"

using namespace rosewood::data_structures;

TEST(VariantTests, BasicLaws) {
    Variant<int, double> i(5);
    Variant<int, double> f(10.0);

    EXPECT_TRUE(i.has<int>());
    EXPECT_TRUE(f.has<double>());

    EXPECT_FALSE(i.has<double>());
    EXPECT_FALSE(f.has<int>());

    EXPECT_EQ(5,     i.get<int>());
    EXPECT_EQ(10.0f, f.get<double>());
}

TEST(VariantTests, Assignment) {
    Variant<int, double> v(1);

    v = 5.0;

    EXPECT_TRUE(v.has<double>());
    EXPECT_EQ(5.0, v.get<double>());

    v = 2;

    EXPECT_TRUE(v.has<int>());
    EXPECT_EQ(2, v.get<int>());
}

struct Destructable1 {
    bool *flag;
    Destructable1() : flag(nullptr) { }
    ~Destructable1() { if (flag) *flag = true; }
};

struct Destructable2 {
    bool *flag;
    Destructable2() : flag(nullptr) { }
    ~Destructable2() { if (flag) *flag = true; }
};

TEST(VariantTests, DestructorCalledOnDestruction) {
    bool flag1 = false;
    Destructable1 d1;

    {
        Variant<Destructable1, Destructable2> v(d1);
        v.get<Destructable1>().flag = &flag1;
        EXPECT_FALSE(flag1);
    }

    EXPECT_TRUE(flag1);
}

TEST(VariantTests, DestructorCalledOnAssignment) {
    bool flag1 = false, flag2 = false;
    Destructable1 d1;
    Destructable2 d2;
    Variant<Destructable1, Destructable2> v(d1);

    v.get<Destructable1>().flag = &flag1;
    EXPECT_FALSE(flag1);
    EXPECT_TRUE(v.has<Destructable1>());

    v = d2;
    EXPECT_TRUE(v.has<Destructable2>());

    v.get<Destructable2>().flag = &flag2;

    EXPECT_TRUE(flag1);
    EXPECT_FALSE(flag2);

    v = d1;
    EXPECT_TRUE(v.has<Destructable1>());

    EXPECT_TRUE(flag2);
}

TEST(VariantTests, Equality) {
    Variant<int, double> i(3);

    EXPECT_TRUE(i == 3);
    EXPECT_FALSE(i == 3.0);
}

TEST(VariantTests, VariantAssignment) {
    Variant<int, double> d(5.0);
    Variant<int, double> i(2);

    i = d;

    EXPECT_TRUE(i.has<double>());
    EXPECT_EQ(5.0, i.get<double>());
}

TEST(VariantTests, CopyConstruction) {
    Variant<int, double> d1(5.0);
    Variant<int, double> d2(d1);

    EXPECT_TRUE(d2.has<double>());
    EXPECT_EQ(5.0, d2.get<double>());
}
