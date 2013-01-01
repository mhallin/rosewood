#include "rosewood/core/stats.h"

namespace rosewood { namespace core { namespace stats {

    Counter<size_t> draw_calls;
    Counter<size_t> triangle_count;
    Counter<size_t> shader_change_count;

} } }
