#include "rosewood/physics/rigidbody.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/vector.h"
#include "rosewood/math/quaternion.h"

#include "rosewood/core/transform.h"

#include "rosewood/physics/bullet_common.h"
#include "rosewood/physics/shape.h"

using rosewood::math::from_bt;
using rosewood::math::to_bt;

using rosewood::core::Entity;
using rosewood::core::Transform;
using rosewood::core::transform;

using rosewood::physics::Rigidbody;

Rigidbody::Rigidbody(Entity owner, btDiscreteDynamicsWorld *world, float mass)
: core::Component<Rigidbody>(owner)
{
    auto world_rot = transform(owner)->world_rotation();
    auto world_pos = transform(owner)->world_position();

    _motion_state = btDefaultMotionState(to_bt(world_pos, world_rot));

    reload_shape();

    btVector3 inertia;
    _shape.calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo info(mass, &_motion_state, &_shape, inertia);
    
    _rigidbody = make_unique<btRigidBody>(info);

    if (_shape.getNumChildShapes()) {

        world->addRigidBody(_rigidbody.get());
    }
}

void Rigidbody::synchronize() {
    auto world_transform = _motion_state.m_graphicsWorldTrans;
    auto new_position = from_bt(world_transform.getOrigin());
    auto new_rotation = from_bt(world_transform.getRotation());
    
    auto my_transform = transform(entity());
    if (my_transform->local_position() != new_position ||
        my_transform->local_rotation() != new_rotation) {
        my_transform->set_local_position(new_position);
        my_transform->set_local_rotation(new_rotation);
    }
}

void Rigidbody::reload_shape() {
    while (_shape.getNumChildShapes()) {
        _shape.removeChildShapeByIndex(0);
    }

    add_shapes_from(transform(entity()));
}

void Rigidbody::add_shapes_from(Transform *node) {
    for (auto child : node->children()) {
        add_shapes_from(child);
    }

    auto shape_comp = shape(node->entity());
    if (shape_comp) {
        auto shape = shape_comp->shape();
        auto transform = to_bt(node->local_position(), node->local_rotation());

        _shape.addChildShape(transform, shape.get());
    }
}