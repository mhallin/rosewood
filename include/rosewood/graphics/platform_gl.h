#ifndef __ROSEWOOD_GRAPHICS_PLATFORM_GL_H__
#define __ROSEWOOD_GRAPHICS_PLATFORM_GL_H__

#include <rosewood/target.h>

#if TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES

#define SHADER_EXT "es2"

#endif

#if TARGET_OS_MAC && !TARGET_OS_IPHONE
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#define SHADER_EXT "gl32"

#endif

#if TARGET_WEBGL

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define SHADER_EXT "es2"

#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES

#endif

#if TARGET_OS_UNIX

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

#define SHADER_EXT "gl32"

#endif

#endif
