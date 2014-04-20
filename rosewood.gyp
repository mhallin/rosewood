{
    "includes": [
        "common.gypi",
    ],

    "targets": [
        {
            "target_name": "rw_gtest",
            "product_name": "rw_gtest",

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

            "sources": [
                "deps/build/gtest-<(OS)/src/gtest-all.cc"
            ],

            "include_dirs": [
                "deps/build/gtest-<(OS)/include",
                "deps/build/gtest-<(OS)",
            ],

            "direct_dependent_settings": {
                "include_dirs": [
                    "deps/build/gtest-<(OS)/include",
                ],
            },

            "xcode_settings": {
                "GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS": "NO",
                "GCC_WARN_ABOUT_MISSING_PROTOTYPES": "NO",
            },

            "target_conditions": [
                [
                    "OS == 'emscripten'",
                    {
                        "include_dirs": [
                            "deps/emsdk_portable/emscripten/1.13.0/system/lib/libcxxabi/include",
                        ],
                    }
                ],
            ],
        },

        {
            "target_name": "rw_tests",
            "type": "executable",

            "includes": [
                "tests/sources.gypi",
            ],

            "dependencies": [
                "engine/engine.gyp:rw_math",
                "engine/engine.gyp:rw_core",
                "engine/engine.gyp:rw_data_format",
                "rw_gtest",
            ],
 
             "target_conditions": [
                [
                    "OS == 'emscripten'",
                    {
                        "product_name": "rw_tests.html",

                        "include_dirs": [
                            "deps/emsdk_portable/emscripten/1.13.0/system/lib/libcxxabi/include",
                        ],

                        "postbuilds": [
                            {
                                "postbuild_name": "Copy test runner script",
                                "action": [
                                    "cp", "tests/run_rw_tests.sh", "${BUILT_PRODUCTS_DIR}/rw_tests",
                                ],
                            }
                        ],
                    }
                ],
            ],
       },
    ],
}