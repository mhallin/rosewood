{
    "includes": [
        "../common.gypi",
    ],

    "target_defaults": {
        "type": "static_library",

        "include_dirs": [
            "../include",
        ],

        "direct_dependent_settings": {
            "include_dirs": [
                "../include",
            ],
        },
    },

    "targets": [
        {
            "target_name": "rw_core",
            "product_name": "rw_core",

            "includes": [
                "core/sources.gypi",
            ],
        },

        {
            "target_name": "rw_graphics",
            "product_name": "rw_graphics",

            "include_dirs": [
                "../deps/build/msgpack-<(OS)/include",
            ],

            "includes": [
                "graphics/sources.gypi",
            ],

            "direct_dependent_settings": {
                "libraries": [
                    "-lmsgpack",
                    "$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework",
                ],

                "xcode_settings": {
                    "OTHER_LDFLAGS": [
                        "-L../deps/build/msgpack-<(OS)/lib",
                    ],
                },
            },
        },

        {
            "target_name": "rw_math",
            "product_name": "rw_math",

            "includes": [
                "math/sources.gypi",
            ],
        },

        {
            "target_name": "rw_platform",
            "product_name": "rw_platform",

            "includes": [
                "platform/sources.gypi",
            ],
        },

        {
            "target_name": "rw_utils",
            "product_name": "rw_utils",

            "includes": [
                "utils/sources.gypi",
            ],
        },
    ],
}