#ifndef __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_EMITTER_H__
#define __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_EMITTER_H__

#include "rosewood/core/component.h"

namespace rosewood { namespace particle_system {

    class ParticleEmitter : public core::Component<ParticleEmitter> {
    public:
        ParticleEmitter(core::Entity owner);
    };

} }

#endif
