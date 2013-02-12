#include "rosewood/physics/shape.h"

#include "rosewood/physics/bullet_common.h"

using rosewood::core::Entity;

using rosewood::physics::Shape;

Shape::Shape(Entity owner, const std::shared_ptr<btCollisionShape> &shape)
: core::Component<Shape>(owner)
, _shape(shape)
{
    _shape->setUserPointer(this);
}

void Shape::set_shape(const std::shared_ptr<btCollisionShape> &shape) {
    _shape = shape;
    _shape->setUserPointer(this);
}

Shape *Shape::from_collision_shape(const btCollisionShape *shape) {
    return static_cast<Shape*>(shape->getUserPointer());
}
