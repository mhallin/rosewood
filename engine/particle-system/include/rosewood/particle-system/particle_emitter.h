#ifndef __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_EMITTER_H__
#define __ROSEWOOD_PARTICLE_SYSTEM_PARTICLE_EMITTER_H__

#include "rosewood/core/component.h"
#include "rosewood/core/memory.h"

#include "rosewood/data-structures/variant.h"

#include "rosewood/utils/time.h"

namespace rosewood { namespace graphics {

    class Mesh;
    class Material;

} }

namespace rosewood { namespace particle_system {

    struct PointArea { math::Vector3 point; };
    struct SphereArea { math::Vector3 center; float radius; };
    struct BoxArea { math::Vector3 min_extent; math::Vector3 max_extent; };

    typedef data_structures::Variant<PointArea, SphereArea, BoxArea> ParticleEmitterArea;

    class ParticleEmitter : public core::Component<ParticleEmitter> {
    public:
        ParticleEmitter(core::Entity owner)
        : core::Component<ParticleEmitter>(owner)
        , is_enabled(true)
        , accumulated_downtime(0) { }

        std::shared_ptr<graphics::Mesh> mesh;
        std::shared_ptr<graphics::Material> material;

        ParticleEmitterArea emission_area;

        math::Vector3 direction;
        math::Vector3 direction_random_range;

        float velocity;
        float velocity_random_range;

        float emission_rate;

        bool is_enabled;

        utils::UsecTime accumulated_downtime;
    };

} }

#endif
