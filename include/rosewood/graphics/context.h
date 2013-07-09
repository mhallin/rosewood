#ifndef __ROSEWOOD_GRAPHICS_CONTEXT_H__
#define __ROSEWOOD_GRAPHICS_CONTEXT_H__

#include "rosewood/core/memory.h"

namespace rosewood { namespace graphics {

    class SharedGraphicsContext;

    std::unique_ptr<SharedGraphicsContext> make_shared_graphics_context();

    class SharedGraphicsContext {
    public:
        SharedGraphicsContext();
        virtual ~SharedGraphicsContext() = 0;
    };

} }

#endif
