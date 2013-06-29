#ifndef __ROSEWOOD_TARGET_H__
#define __ROSEWOOD_TARGET_H__

#ifdef __APPLE__

#include <TargetConditionals.h>

#else

#define TARGET_OS_UNIX 1

#endif


#endif
