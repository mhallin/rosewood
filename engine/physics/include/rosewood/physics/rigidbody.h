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
        Rigidbody(core::Entity owner, btDynamicsWorld *world, float mass,
                  short collision_group, short collision_mask);
        virtual ~Rigidbody();

        void reload_shape();

        void debug_draw(btIDebugDraw *renderer);

        btRigidBody *rigidbody() { return &_rigidbody; }

        // This rigidbody belongs to the following groups [bitmask]
        short collision_groups() const {
            return _rigidbody.getBroadphaseProxy()->m_collisionFilterGroup;
        }

        void set_collision_groups(short groups) {
            _rigidbody.getBroadphaseProxy()->m_collisionFilterGroup = groups;
        }

        // This rigidbody collides with the following groups [bitmask]
        short collision_mask() const {
            return _rigidbody.getBroadphaseProxy()->m_collisionFilterMask;
        }

        void set_collision_mask(short mask) {
            _rigidbody.getBroadphaseProxy()->m_collisionFilterMask = mask;
        }

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
