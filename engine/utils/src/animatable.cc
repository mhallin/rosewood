#include "rosewood/utils/animatable.h"

#include <unordered_set>

using rosewood::utils::BaseAnimation;

static std::unordered_set<BaseAnimation*> gRegisteredAnimations;

static void register_animation(BaseAnimation *anim) {
    gRegisteredAnimations.insert(anim);
}

static void unregister_animation(BaseAnimation *anim) {
    gRegisteredAnimations.erase(anim);
}

void rosewood::utils::tick_all_animations() {
    for (auto &anim : gRegisteredAnimations) {
        anim->tick();
    }
}

BaseAnimation::BaseAnimation() {
    register_animation(this);
}

BaseAnimation::~BaseAnimation() {
    unregister_animation(this);
}
