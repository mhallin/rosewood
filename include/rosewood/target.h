#ifndef __ROSEWOOD_TARGET_H__
#define __ROSEWOOD_TARGET_H__

#if defined(__APPLE__) && !defined(EMSCRIPTEN)

#    include <TargetConditionals.h>

#elif defined(EMSCRIPTEN)

#    define TARGET_WEBGL 1

#else

#    define TARGET_OS_UNIX 1

#endif


#endif
