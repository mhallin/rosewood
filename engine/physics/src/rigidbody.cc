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

Rigidbody::Rigidbody(Entity owner, btDynamicsWorld *world, float mass,
                     short collision_group, short collision_mask)
: core::Component<Rigidbody>(owner)
, _rigidbody(mass, this, &_shape)
{
    reload_shape();

    btVector3 inertia;
    _shape.calculateLocalInertia(mass, inertia);

    _rigidbody.setMassProps(mass, inertia);
    _rigidbody.setRollingFriction(0.3f);
    _rigidbody.setFriction(0.8f);
    _rigidbody.setUserPointer(this);

    if (_shape.getNumChildShapes()) {
        world->addRigidBody(&_rigidbody, collision_group, collision_mask);
    }
}

Rigidbody::~Rigidbody() {
}

void Rigidbody::setWorldTransform(const btTransform &world_tform) {
    auto my_transform = transform(entity());
    auto new_position = from_bt(world_tform.getOrigin());
    auto new_rotation = from_bt(world_tform.getRotation());

    if (my_transform->local_position() != new_position
        || my_transform->local_rotation() != new_rotation) {
        my_transform->set_local_position(new_position);
        my_transform->set_local_rotation(new_rotation);
    }
}

void Rigidbody::getWorldTransform(btTransform &world_tform) const {
    world_tform = to_bt(transform(entity()));
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
