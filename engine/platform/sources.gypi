{
    "target_conditions": [
        [
            "OS == 'mac'",
            {
                "sources": [
                    "include/rosewood/platform/glview_osx.h",
                    "src/glview_osx.mm",
                ],
            }
        ],
        [
            "OS == 'ios'",
            {
                "sources": [
                    "include/rosewood/platform/glviewcontroller_ios.h",
                    "src/glviewcontroller_ios.mm",
                ],
            }
        ],
        [
            "OS == 'emscripten'",
            {
                "sources": [
                    "src/dummy_emscripten.cc",
                ],
            }
        ],
    ],
}