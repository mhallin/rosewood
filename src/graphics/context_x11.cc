#include "rosewood/graphics/context.h"

#define GLX_GLXEXT_PROTOTYPES

#include <GL/glx.h>
#include <GL/glxext.h>

#include <functional>

std::function<GLXContext()> gGLXContextConstructor;
std::function<void(GLXContext)> gGLXContextActivator;
std::function<void(GLXContext)> gGLXContextDestructor;

namespace rosewood { namespace graphics {

    class SharedGLXContext : public SharedGraphicsContext {
    public:
        SharedGLXContext()
		: _local_context(gGLXContextConstructor())
        {
			gGLXContextActivator(_local_context);
        }

        virtual ~SharedGLXContext() {
			gGLXContextDestructor(_local_context);
        }

    private:
		GLXContext _local_context;
    };

} }

using rosewood::graphics::SharedGraphicsContext;

SharedGraphicsContext::SharedGraphicsContext() { }
SharedGraphicsContext::~SharedGraphicsContext() { }

std::unique_ptr<SharedGraphicsContext> rosewood::graphics::make_shared_graphics_context() {
    return make_unique<SharedGLXContext>();
}
