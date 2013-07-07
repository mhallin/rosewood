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

    SecTime current_time() {
        return current_usec_time() / (SecTime)kUsecPerSec;
    }
    
    SecTime delta_time() {
        return (gCurrentFrameTime - gLastFrameTime) / (SecTime)kUsecPerSec;
    }
    
    uint64_t frame_usec_time() {
        return gCurrentFrameTime - gFirstFrameTime;
    }
    
    FSecTime frame_time() {
        return (gCurrentFrameTime - gFirstFrameTime) / (FSecTime)kUsecPerSec;
    }
    
    void mark_frame_beginning() {
        auto new_current = current_usec_time();
        
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
