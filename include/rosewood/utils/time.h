#ifndef __ROSEWOOD_ENGINE_TIME_H__
#define __ROSEWOOD_ENGINE_TIME_H__

#include <stdint.h>

namespace rosewood { namespace utils {
    
    const uint64_t kUsecPerSec = 1000000;

    uint64_t current_usec_time();
    float current_time();
    
    uint64_t frame_usec_time();
    float frame_time(); // Absolute timepoint for the current frame

    float delta_time(); // Time it took to render the last frame
    
    void mark_frame_beginning();
    
} }

#endif
