#ifndef __ROSEWOOD_PHYSICS_SHAPE_H__
#define __ROSEWOOD_PHYSICS_SHAPE_H__

#include "rosewood/core/entity.h"
#include "rosewood/core/memory.h"
#include "rosewood/core/component.h"

class btCollisionShape;

namespace rosewood { namespace physics {

    class Shape;

    Shape *shape(core::Entity entity);

    class Shape : public core::Component<Shape> {
    public:
        Shape(core::Entity owner, const std::shared_ptr<btCollisionShape> &shape);

        const std::shared_ptr<btCollisionShape> &shape() const;
        void set_shape(const std::shared_ptr<btCollisionShape> &shape);

        static Shape *from_collision_shape(const btCollisionShape *shape);

    private:
        std::shared_ptr<btCollisionShape> _shape;
    };

    inline Shape *shape(core::Entity entity) {
        return entity.component<Shape>();
    }

    inline const std::shared_ptr<btCollisionShape> &Shape::shape() const { return _shape; }

} }

#endif
