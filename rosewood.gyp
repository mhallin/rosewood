{
    "includes": [
        "common.gypi",
    ],

    "targets": [
        {
            "target_name": "rw_gtest",
            "product_name": "rw_gtest",
            "type": "static_library",

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
        },

        {
            "target_name": "rw_tests",
            "type": "executable",

            "includes": [
                "tests/sources.gypi",
            ],

            "dependencies": [
                "src/engine.gyp:rw_math",
                "src/engine.gyp:rw_core",
                "rw_gtest",
            ],
        },
    ],
}