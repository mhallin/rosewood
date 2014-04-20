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
    },

    "targets": [
        {
            "target_name": "rw_core",
            "product_name": "rw_core",

            "includes": [
                "core/sources.gypi",
            ],

            "include_dirs": [
                "core/include",
            ],

            "all_dependent_settings": {
                "include_dirs": [
                    "core/include",
                ],
            },

            "dependencies": [
                "rw_math",
            ],
        },

        {
            "target_name": "rw_data_format",
            "product_name": "rw_data_format",

            "includes": [
                "data-format/sources.gypi",
            ],

            "include_dirs": [
                "data-format/include",
            ],

            "all_dependent_settings": {
                "include_dirs": [
                    "data-format/include",
                ],
            },
        },

        {
            "target_name": "rw_graphics",
            "product_name": "rw_graphics",

            "includes": [
                "graphics/sources.gypi",
            ],

            "include_dirs": [
                "graphics/include",
            ],

            "dependencies": [
                "rw_core",
                "rw_data_format",
                "rw_math",
            ],

            "conditions": [
                [
                    "OS == 'mac' or OS == 'ios'",
                    {
                        "all_dependent_settings": {
                            "libraries": [
                                "$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework",
                            ],
                        },
                    }
                ],
            ],

            "all_dependent_settings": {
                "include_dirs": [
                    "graphics/include",
                ],
            },
        },

        {
            "target_name": "rw_math",
            "product_name": "rw_math",

            "includes": [
                "math/sources.gypi",
            ],

            "include_dirs": [
                "math/include",
            ],

            "all_dependent_settings": {
                "include_dirs": [
                    "math/include",
                ],
            },
        },

        {
            "target_name": "rw_particle_system",
            "product_name": "rw_particle_system",

            "includes": [
                "particle-system/sources.gypi",
            ],

            "include_dirs": [
                "particle-system/include",
            ],

            "dependencies": [
                "rw_core",
                "rw_graphics",
            ],

            "all_dependent_settings": {
                "include_dirs": [
                    "particle-system/include",
                ],
            },
        },

        {
            "target_name": "rw_platform",
            "product_name": "rw_platform",

            "includes": [
                "platform/sources.gypi",
            ],

            "include_dirs": [
                "platform/include",
            ],

            "dependencies": [
                "rw_core",
                "rw_utils",
            ],

            "all_dependent_settings": {
                "include_dirs": [
                    "platform/include",
                ],
            },
        },

        {
            "target_name": "rw_utils",
            "product_name": "rw_utils",

            "includes": [
                "utils/sources.gypi",
            ],

            "include_dirs": [
                "utils/include",
            ],

            "dependencies": [
                "rw_core",
                "rw_graphics",
                "rw_math",
            ],

            "all_dependent_settings": {
                "include_dirs": [
                    "utils/include",
                ],
            },
        },
    ],
}