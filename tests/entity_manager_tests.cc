#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>

#include "rosewood/core/entity.h"
#include "rosewood/core/component.h"

using namespace rosewood::core;

class EntityManagerTests : public ::testing::Test {
protected:
    EntityManager _entities;
};

TEST_F(EntityManagerTests, CreateEntity) {
    Entity e = _entities.create_entity();
    EXPECT_TRUE(_entities.is_valid(e));
    EXPECT_EQ(1, _entities.entity_count());
}

TEST_F(EntityManagerTests, DestroyEntity) {
    Entity e = _entities.create_entity();
    _entities.destroy_entity(e);
    EXPECT_FALSE(_entities.is_valid(e));
    EXPECT_EQ(0, _entities.entity_count());
}

TEST_F(EntityManagerTests, ReuseId) {
    Entity e1 = _entities.create_entity();
    _entities.create_entity();
    
    _entities.destroy_entity(e1);
    
    Entity e3 = _entities.create_entity();
    
    EXPECT_EQ(e1.eid, e3.eid);
}

struct TestComponent : public Component<TestComponent> {
    TestComponent(Entity owner) : Component<TestComponent>(owner) { }
};

struct TestComponent2 : public Component<TestComponent2> {
    TestComponent2(Entity owner) : Component<TestComponent2>(owner) { }
};

struct CtorDtorComponent : public Component<CtorDtorComponent> {
    CtorDtorComponent(Entity owner, bool *ctor_called, bool *dtor_called)
    : Component<CtorDtorComponent>(owner)
    , _dtor_called(dtor_called)
    {
        *ctor_called = true;
    }
    
    ~CtorDtorComponent() {
        *_dtor_called = true;
    }
    
private:
    bool *_dtor_called;
};

TEST_F(EntityManagerTests, AddComponent) {
    Entity e1 = _entities.create_entity();
    
    auto comp = _entities.add_component<TestComponent>(e1);
    ASSERT_NE(nullptr, comp);
}

TEST_F(EntityManagerTests, GetComponent) {
    Entity e1 = _entities.create_entity();
    Entity e2 = _entities.create_entity();
    auto comp = _entities.add_component<TestComponent>(e1);
    
    ASSERT_EQ(comp, _entities.component<TestComponent>(e1));
    ASSERT_EQ(nullptr, _entities.component<TestComponent>(e2));
}

TEST_F(EntityManagerTests, AutoAddComponent) {
    Entity e1 = _entities.create_entity<TestComponent>();
    
    auto comp = _entities.component<TestComponent>(e1);
    ASSERT_NE(nullptr, comp);
}

TEST_F(EntityManagerTests, AutoAddManyComponents) {
    Entity e1 = _entities.create_entity<TestComponent, TestComponent2>();
    
    ASSERT_NE(nullptr, _entities.component<TestComponent>(e1));
    ASSERT_NE(nullptr, _entities.component<TestComponent2>(e1));
}

TEST_F(EntityManagerTests, RemoveComponent) {
    bool ctor_called = false, dtor_called = false;
    Entity e1 = _entities.create_entity();
    
    _entities.add_component<CtorDtorComponent>(e1, &ctor_called, &dtor_called);
    
    EXPECT_TRUE(ctor_called);
    EXPECT_FALSE(dtor_called);
    
    _entities.remove_component<CtorDtorComponent>(e1);
    
    EXPECT_TRUE(dtor_called);
    EXPECT_EQ(nullptr, _entities.component<CtorDtorComponent>(e1));
}

TEST_F(EntityManagerTests, RemoveComponentsWhenEntityRemoved) {
    bool ctor_called = false, dtor_called = false;
    Entity e1 = _entities.create_entity();
    
    _entities.add_component<CtorDtorComponent>(e1, &ctor_called, &dtor_called);
    EXPECT_NE(nullptr, _entities.component<CtorDtorComponent>(e1));

    _entities.destroy_entity(e1);
    EXPECT_TRUE(dtor_called);
}

struct __attribute__ ((aligned (1024))) AlignedComponent : public Component<AlignedComponent> {
    AlignedComponent(Entity entity, int i) : Component<AlignedComponent>(entity), i(i) { }
    
    int i;
};

TEST_F(EntityManagerTests, AlignedComponentsSanity) {
    auto e1 = _entities.create_entity();
    AlignedComponent ac(e1, 123);
    
    ASSERT_EQ(0, size_t(&ac) % 1024);
}

TEST_F(EntityManagerTests, AlignedComponent) {
    auto e1 = _entities.create_entity();
    auto e2 = _entities.create_entity();
    auto e3 = _entities.create_entity();
    auto e4 = _entities.create_entity();

    auto c1 = _entities.add_component<AlignedComponent>(e1, 123);
    auto c2 = _entities.add_component<AlignedComponent>(e2, 123);
    auto c3 = _entities.add_component<AlignedComponent>(e3, 123);
    auto c4 = _entities.add_component<AlignedComponent>(e4, 123);
    
    EXPECT_EQ(0, size_t(c1) % 1024);
    EXPECT_EQ(0, size_t(c2) % 1024);
    EXPECT_EQ(0, size_t(c3) % 1024);
    EXPECT_EQ(0, size_t(c4) % 1024);
}
