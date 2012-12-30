#include "rosewood/utils/time.h"

#include <sys/time.h>

static uint64_t gFirstFrameTime = 0;
static uint64_t gLastFrameTime = 0;
static uint64_t gCurrentFrameTime = 0;

namespace rosewood { namespace utils {
    
    uint64_t current_usec_time() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        
        return kUsecPerSec * tv.tv_sec + tv.tv_usec;
    }
    
    float delta_time() {
        return (gCurrentFrameTime - gLastFrameTime) / (float)kUsecPerSec;
    }
    
    uint64_t frame_usec_time() {
        return gCurrentFrameTime - gFirstFrameTime;
    }
    
    float frame_time() {
        return (gCurrentFrameTime - gFirstFrameTime) / (float)kUsecPerSec;
    }
    
    void mark_frame_beginning() {
        uint64_t new_current = current_usec_time();
        
        if (!gFirstFrameTime) {
            gFirstFrameTime = new_current;
        }

        if (!gLastFrameTime) {
            gLastFrameTime = gCurrentFrameTime = new_current;
        }
        else {
            gLastFrameTime = gCurrentFrameTime;
            gCurrentFrameTime = new_current;
        }
    }
    
} }
