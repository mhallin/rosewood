#include "rosewood/core/assert.h"

#include "rosewood/core/logging.h"

void rosewood::core::assert::fail(const char *filename, int line_number, const char *msg, ...) {
    LOG(ERROR) << filename << ":" << line_number << ": Assertion failure:";

    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    fprintf(stderr, "\n");
    fflush(stderr);

    abort();
}
