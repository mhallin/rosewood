{
    "sources": [
        "include/rosewood/graphics/camera.h",
        "include/rosewood/graphics/context.h",
        "include/rosewood/graphics/gl_func.h",
        "include/rosewood/graphics/gl_state.h",
        "include/rosewood/graphics/image_loader.h",
        "include/rosewood/graphics/light.h",
        "include/rosewood/graphics/material.h",
        "include/rosewood/graphics/mesh.h",
        "include/rosewood/graphics/platform_gl.h",
        "include/rosewood/graphics/render_queue.h",
        "include/rosewood/graphics/renderable.h",
        "include/rosewood/graphics/shader.h",
        "include/rosewood/graphics/texture.h",
        "include/rosewood/graphics/view_frustum.h",

        "src/camera.cc",
        "src/gl_func.cc",
        "src/gl_state.cc",
        "src/material.cc",
        "src/mesh.cc",
        "src/render_queue.cc",
        "src/shader.cc",
        "src/texture.cc",
        "src/view_frustum.cc",
    ],

    "target_conditions": [
        [
            "OS == 'mac'",
            {
                "sources": [
                    "src/image_loader_osx.mm",
                ],
            }
        ],
        [
            "OS == 'ios'",
            {
                "sources": [
                    "src/image_loader_ios.mm",
                ],
            }
        ],
        [
            "OS == 'emscripten'",
            {
                "sources": [
                    "src/image_loader_sdl.cc",
                ],
            }
        ],
    ],
}