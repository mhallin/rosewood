{
    "sources": [
        "include/rosewood/utils/animatable-impl.h",
        "include/rosewood/utils/animatable.h",
        "include/rosewood/utils/debug.h",
        "include/rosewood/utils/folder_resource_loader.h",
        "include/rosewood/utils/interpolate.h",
        "include/rosewood/utils/render_system.h",
        "include/rosewood/utils/time.h",

        "src/animatable.cc",
        "src/debug.cc",
        "src/folder_resource_loader.cc",
        "src/render_system.cc",
        "src/time.cc",
    ],

    "target_conditions": [
        [
            "OS == 'mac' or OS == 'ios'",
            {
                "sources": [
                    "include/rosewood/utils/bundle_resource_loader_ns.h",
                    "src/bundle_resource_loader_ns.mm",
                ],
            }
        ],
    ],
}