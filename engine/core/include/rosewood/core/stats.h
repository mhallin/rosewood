#ifndef __ROSEWOOD_CORE_STATS_H__
#define __ROSEWOOD_CORE_STATS_H__

#include <stddef.h>

namespace rosewood { namespace core {

    namespace stats {

        template<typename T>
        class Counter {
        public:
            T read() const { return _count; }
            void increment() { _count += T(1); }
            void increment(T amount) { _count += amount; }
            void reset() { _count = T(0); }

        private:
            T _count;
        };

        extern Counter<size_t> draw_calls;
        extern Counter<size_t> triangle_count;
        extern Counter<size_t> shader_change_count;

        extern size_t render_queue_size;

        extern int debug_single_draw_call_index;
        extern bool debug_single_draw_call_enabled;
    }

} }

#endif
