#ifndef __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_H__
#define __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_H__

#include "rosewood/core/Component.h"

namespace rosewood { namespace particle_system {

    class Particle : public core::Component<Particle> {
    public:
        Particle(core::Entity owner);
    };

} }

#endif
