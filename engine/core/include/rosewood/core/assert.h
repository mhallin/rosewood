#ifndef __ROSEWOOD_CORE_ASSERT_H__
#define __ROSEWOOD_CORE_ASSERT_H__

namespace rosewood { namespace core {

    namespace assert {

        void fail(const char *filename, int line_number, const char *msg, ...) __attribute__((noreturn));

    }

} }

#if ENABLE_RW_ASSERTIONS

#    define RW_ASSERT(e, ...) do { if (__builtin_expect(!(e), 0)) { \
       rosewood::core::assert::fail(__FILE__, __LINE__, __VA_ARGS__); } } while (0)

#else

#    define RW_ASSERT(e)

#endif

#define RW_UNREACHABLE(...) rosewood::core::assert::fail(__FILE__, __LINE__, __VA_ARGS__)

#endif
