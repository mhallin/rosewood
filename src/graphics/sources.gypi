{
    "sources": [
        "camera.cc",
        "gl_func.cc",
        "gl_state.cc",
        "material.cc",
        "mesh.cc",
        "render_queue.cc",
        "shader.cc",
        "texture.cc",
        "view_frustum.cc",
    ],

    "target_conditions": [
        [
            "OS == 'mac'",
            {
                "sources": [
                    "image_loader_osx.mm",
                ],
            }
        ],
        [
            "OS == 'ios'",
            {
                "sources": [
                    "image_loader_ios.mm",
                ],
            }
        ],
        [
            "OS == 'emscripten'",
            {
                "sources": [
                    "image_loader_sdl.cc",
                ],
            }
        ],
    ],
}