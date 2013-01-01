#include "rosewood/core/stats.h"

namespace rosewood { namespace core { namespace stats {

    Counter<int> draw_calls;
    Counter<int> triangle_count;
    Counter<int> shader_change_count;

} } }
