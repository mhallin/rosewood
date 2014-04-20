#include "rosewood/particle-system/particle_system.h"

#include "rosewood/core/entity.h"
#include "rosewood/core/transform.h"

#include "rosewood/particle-system/particle.h"
#include "rosewood/particle-system/particle_emitter.h"

using rosewood::core::EntityManager;
using rosewood::core::Transform;

using rosewood::particle_system::Particle;
using rosewood::particle_system::ParticleEmitter;

static void step_particle(Transform *tform, Particle *particle) {

}

static bool is_particle_decayed(Particle *particle) {
    return false;
}

static void remove_particle(Particle *particle) {
    particle->entity().destroy();
}

static void step_particle_emitter(Transform *transform, ParticleEmitter *emitter) {

}

void rosewood::particle_system::particle_system::update(EntityManager *entities) {
    entities->for_components<Transform, Particle>([](Transform *tform, Particle *particle) {
        step_particle(tform, particle);

        if (is_particle_decayed(particle)) {
            remove_particle(particle);
        }
    });

    entities->for_components<Transform, ParticleEmitter>([](Transform *tform, ParticleEmitter *emitter) {
        step_particle_emitter(tform, emitter);
    });
}
