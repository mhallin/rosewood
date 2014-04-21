#ifndef __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_H__
#define __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_H__

#include "rosewood/core/component.h"

#include "rosewood/utils/time.h"

namespace rosewood { namespace particle_system {

    class Particle : public core::Component<Particle> {
    public:
        Particle(core::Entity owner) : core::Component<Particle>(owner) { };

        utils::UsecTime decay_time;

        math::Vector3 velocity;
    };

} }

#endif
