{
    "includes": [
        "../common.gypi",
    ],

    "target_defaults": {
        "conditions": [
            [
                "OS == 'emscripten'",
                {
                    "type": "shared_library",
                }
            ],
            [
                "OS != 'emscripten'",
                {
                    "type": "static_library",
                }
            ],
        ],

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
                "../include/rosewood/core/sources.gypi",
                "../include/rosewood/data-structures/sources.gypi",
            ],
        },

        {
            "target_name": "rw_data_format",
            "product_name": "rw_data_format",

            "includes": [
                "data-format/sources.gypi",
                "../include/rosewood/data-format/sources.gypi",
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
                "../include/rosewood/graphics/sources.gypi",
            ],

            "dependencies": [
                "rw_data_format",
            ],

            "conditions": [
                [
                    "OS == 'mac' or OS == 'ios'",
                    {
                        "direct_dependent_settings": {
                            "libraries": [
                                "$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework",
                            ],
                        },
                    }
                ]
            ],
        },

        {
            "target_name": "rw_math",
            "product_name": "rw_math",

            "includes": [
                "math/sources.gypi",
                "../include/rosewood/math/sources.gypi",
            ],
        },

        {
            "target_name": "rw_platform",
            "product_name": "rw_platform",

            "includes": [
                "platform/sources.gypi",
                "../include/rosewood/platform/sources.gypi",
            ],
        },

        {
            "target_name": "rw_utils",
            "product_name": "rw_utils",

            "includes": [
                "utils/sources.gypi",
                "../include/rosewood/utils/sources.gypi",
            ],
        },
    ],
}