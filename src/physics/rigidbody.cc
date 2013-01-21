#include "rosewood/physics/rigidbody.h"

#include <iostream>

#include "rosewood/math/math_types.h"
#include "rosewood/math/vector.h"
#include "rosewood/math/quaternion.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/math_ostream.h"

#include "rosewood/core/transform.h"

#include "rosewood/physics/bullet_common.h"
#include "rosewood/physics/shape.h"

using rosewood::math::Vector3;
using rosewood::math::from_bt;
using rosewood::math::to_bt;
using rosewood::math::quaternion_identity;

using rosewood::core::Entity;
using rosewood::core::Transform;
using rosewood::core::transform;

using rosewood::physics::Rigidbody;
using rosewood::physics::shape;

static void add_shapes_from(btCompoundShape *destination, Transform *root, Transform *node) {
    for (auto child : node->children()) {
        add_shapes_from(destination, root, child);
    }
    
    auto shape_comp = shape(node->entity());
    if (shape_comp) {
        auto shape = shape_comp->shape();
        auto shape_pos = node->convert_to(Vector3(0, 0, 0), root);
        auto shape_rot = node->convert_to(quaternion_identity(), root);
        auto transform = to_bt(shape_pos, shape_rot);
        
        destination->addChildShape(transform, shape.get());
    }
}

Rigidbody::Rigidbody(Entity owner, btDiscreteDynamicsWorld *world, float mass)
: core::Component<Rigidbody>(owner)
{
    reload_shape();

    auto world_rot = transform(owner)->world_rotation();
    auto world_pos = transform(owner)->world_position();

    _motion_state = btDefaultMotionState(to_bt(world_pos, world_rot));
    
    btVector3 inertia;
    _shape.calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo info(mass, &_motion_state, &_shape, inertia);
    
    _rigidbody = make_unique<btRigidBody>(info);
    _rigidbody->setRollingFriction(0.3f);
    _rigidbody->setFriction(0.8f);
    _rigidbody->setUserPointer(this);

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

    add_shapes_from(&_shape, transform(entity()), transform(entity()));
}

void Rigidbody::debug_draw(btIDebugDraw *renderer) {
    auto center = transform(entity())->world_position();
    
    renderer->drawLine(to_bt(center), to_bt(center) + rigidbody()->getTotalForce(), btVector3(0, 1, 1));
}
