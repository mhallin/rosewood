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

    class Rigidbody : public core::Component<Rigidbody> {
    public:
        Rigidbody(core::Entity owner,
                  btDiscreteDynamicsWorld *world,
                  float mass);

        void synchronize();
        void reload_shape();
        
        btRigidBody *rigidbody() { return _rigidbody.get(); }

    private:
        btCompoundShape _shape;
        btDefaultMotionState _motion_state;

        std::unique_ptr<btRigidBody> _rigidbody;
    };
    
    inline Rigidbody *rigidbody(core::Entity entity) {
        return entity.component<Rigidbody>();
    }

} }

#endif
