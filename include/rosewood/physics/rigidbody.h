#ifndef __ROSEWOOD_PHYSICS_RIGIDBODY_H__
#define __ROSEWOOD_PHYSICS_RIGIDBODY_H__

#include "rosewood/core/entity.h"
#include "rosewood/core/memory.h"
#include "rosewood/core/component.h"

#include "rosewood/physics/bullet_common.h"

namespace rosewood { namespace core {
    class Transform;
} }

namespace rosewood { namespace physics {

    class Rigidbody;

    Rigidbody *rigidbody(core::Entity entity);

    class Rigidbody : public core::Component<Rigidbody>, public btMotionState {
    public:
        Rigidbody(core::Entity owner, btDynamicsWorld *world, float mass);
        virtual ~Rigidbody();

        void reload_shape();

        void debug_draw(btIDebugDraw *renderer);

        btRigidBody *rigidbody() { return &_rigidbody; }

        // btMotionState interface
        virtual void getWorldTransform(btTransform& worldTrans) const override;
        virtual void setWorldTransform(const btTransform& worldTrans) override;

    private:
        btCompoundShape _shape;

        btRigidBody _rigidbody;
    };

    inline Rigidbody *rigidbody(core::Entity entity) {
        return entity.component<Rigidbody>();
    }

} }

#endif
