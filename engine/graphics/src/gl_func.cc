#include "rosewood/graphics/gl_func.h"

#include "rosewood/core/logging.h"

void rosewood::graphics::dump_gl_errors() {
    GLenum err = GL_NO_ERROR;
    
    while ((err = glGetError()) != GL_NO_ERROR) {
        LOG(ERROR) << "GL Error " << err << ": " << format_gl_error(err);
    }
}

std::string rosewood::graphics::format_gl_error(GLenum err) {
    switch (err) {
        case GL_NO_ERROR: return "no error";
        case GL_INVALID_ENUM: return "invalid enum";
        case GL_INVALID_VALUE: return "invalid_value";
        case GL_INVALID_OPERATION: return "invalid operation";
        case GL_OUT_OF_MEMORY: return "out of memory";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "invalid framebuffer operation";
        default: return "unknown error";
    }
}
