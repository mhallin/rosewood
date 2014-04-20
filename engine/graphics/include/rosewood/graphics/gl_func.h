#ifndef __ROSEWOOD_GRAPHICS_GL_FUNC_H__
#define __ROSEWOOD_GRAPHICS_GL_FUNC_H__

#include <iostream>
#include <functional>

#include "rosewood/graphics/platform_gl.h"

namespace rosewood { namespace graphics {

    std::string format_gl_error(GLenum err);
    void dump_gl_errors();

    template<typename Ret, typename... TS>
    class GLCheckedCall {
    public:
        explicit GLCheckedCall(const std::function<Ret(TS...)> &func) : _func(func) { }
        ~GLCheckedCall() { dump_gl_errors(); }

        Ret operator()(TS... args) { return _func(args...); }

    private:
        std::function<Ret(TS...)> _func;
    };

    template<typename Ret, typename... TS>
    inline GLCheckedCall<Ret, TS...> make_checked(const std::function<Ret(TS...)> &func) {
        return GLCheckedCall<Ret, TS...>(func);
    }

} }

#if CHECK_GL_CALLS
#define GL_FUNC(func) rosewood::graphics::make_checked(std::function<decltype(func)>(&func))
#else
#define GL_FUNC(func) func
#endif

#endif
