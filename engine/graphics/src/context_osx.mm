#include "rosewood/graphics/context.h"

#import "rosewood/osx/RWGLView.h"

namespace rosewood { namespace graphics {

    class SharedNSOpenGLContext : public SharedGraphicsContext {
    public:
        SharedNSOpenGLContext()
        : _pool([[NSAutoreleasePool alloc] init])
        , _local_context([[NSOpenGLContext alloc] initWithFormat:[RWGLView defaultPixelFormat]
                                                    shareContext:[RWGLView defaultGLView].openGLContext])
        {
            [_local_context makeCurrentContext];
        }

        virtual ~SharedNSOpenGLContext() {
            [NSOpenGLContext clearCurrentContext];
            [_local_context release];
            _local_context = nil;

            [_pool release];
            _pool = nil;
        }

    private:
        NSAutoreleasePool *_pool;
        NSOpenGLContext *_local_context;
    };

} }

using rosewood::graphics::SharedGraphicsContext;

SharedGraphicsContext::SharedGraphicsContext() { }
SharedGraphicsContext::~SharedGraphicsContext() { }

std::unique_ptr<SharedGraphicsContext> rosewood::graphics::make_shared_graphics_context() {
    return make_unique<SharedNSOpenGLContext>();
}
