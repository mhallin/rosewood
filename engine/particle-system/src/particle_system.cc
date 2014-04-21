#include "rosewood/particle-system/particle_system.h"

#include "rosewood/core/assert.h"
#include "rosewood/core/entity.h"
#include "rosewood/core/transform.h"
#include "rosewood/core/logging.h"

#include "rosewood/graphics/renderable.h"

#include "rosewood/math/math_utils.h"
#include "rosewood/math/vector.h"

#include "rosewood/particle-system/particle.h"
#include "rosewood/particle-system/particle_emitter.h"

#include "rosewood/utils/time.h"

using rosewood::core::EntityManager;
using rosewood::core::Transform;

using rosewood::graphics::Renderable;

using rosewood::math::random;
using rosewood::math::random_centered;
using rosewood::math::Vector3;

using rosewood::particle_system::Particle;
using rosewood::particle_system::ParticleEmitter;
using rosewood::particle_system::ParticleEmitterArea;
using rosewood::particle_system::PointArea;
using rosewood::particle_system::SphereArea;
using rosewood::particle_system::BoxArea;

static Vector3 generate_starting_point(const ParticleEmitterArea &area) {
    if (area.has<PointArea>()) {
        return area.get<PointArea>().point;
    }
    else if (area.has<SphereArea>()) {
        auto sphere = area.get<SphereArea>();
        auto radius = random(0, sphere.radius);
        auto point = random(Vector3(-1, -1, -1), Vector3(1, 1, 1));

        return point * radius;
    }
    else if (area.has<BoxArea>()) {
        auto box = area.get<BoxArea>();

        return random(box.min_extent, box.max_extent);
    }

    RW_UNREACHABLE("Invalid data in ParticleEmitterArea");
}

static void step_particle(Transform *tform, Particle *particle) {
    auto pos = tform->local_position() + particle->velocity * rosewood::utils::delta_time();
    tform->set_local_position(pos);
}

static bool is_particle_decayed(Particle *particle) {
    return rosewood::utils::frame_usec_time() > particle->decay_time;
}

static void step_particle_emitter(Transform *transform, ParticleEmitter *emitter) {
    EntityManager *entities = transform->entity().owner;

    auto particle_obj = entities->create_entity();
    auto renderable = particle_obj.add_component<Renderable>();
    renderable->set_material(emitter->material);
    renderable->set_mesh(emitter->mesh);

    auto particle_tform = particle_obj.add_component<Transform>();
    particle_tform->set_local_position(transform->local_position() + generate_starting_point(emitter->emission_area));
    particle_tform->set_local_rotation(transform->local_rotation());
    transform->parent()->add_child(particle_tform);

    auto particle = particle_obj.add_component<Particle>();
    particle->decay_time = rosewood::utils::frame_usec_time() + 1 * rosewood::utils::kUsecPerSec;

    auto velocity = random_centered(emitter->velocity, emitter->velocity_random_range);
    auto direction = random_centered(emitter->direction, emitter->direction_random_range);
    particle->velocity = velocity * direction;
}

void rosewood::particle_system::particle_system::update(EntityManager *entities) {
    std::vector<core::Entity> entities_to_destroy;

    entities->for_components<Transform, Particle>([&entities_to_destroy](Transform *tform, Particle *particle) {
        step_particle(tform, particle);

        if (is_particle_decayed(particle)) {
            entities_to_destroy.push_back(particle->entity());
        }
    });

    for (auto entity : entities_to_destroy) {
        entity.destroy();
    }

    entities->for_components<Transform, ParticleEmitter>([](Transform *tform, ParticleEmitter *emitter) {
        step_particle_emitter(tform, emitter);
    });
}
