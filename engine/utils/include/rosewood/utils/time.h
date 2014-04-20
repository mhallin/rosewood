#ifndef __ROSEWOOD_ENGINE_TIME_H__
#define __ROSEWOOD_ENGINE_TIME_H__

#include <stdint.h>

namespace rosewood { namespace utils {

    typedef uint64_t UsecTime;
    typedef double SecTime;
    typedef double FSecTime;
    
    const UsecTime kUsecPerSec = 1000000;

    UsecTime current_usec_time();
    SecTime current_time();
    
    UsecTime frame_usec_time();
    SecTime frame_time(); // Absolute timepoint for the current frame

    FSecTime delta_time(); // Time it took to render the last frame
    
    void mark_frame_beginning();
    
} }

#endif
