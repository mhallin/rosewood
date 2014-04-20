#include "rosewood/core/stats.h"

namespace rosewood { namespace core { namespace stats {

    Counter<size_t> draw_calls;
    Counter<size_t> triangle_count;
    Counter<size_t> shader_change_count;
    size_t render_queue_size;

    int debug_single_draw_call_index;
    bool debug_single_draw_call_enabled = false;

} } }
