{
    "sources": [
        "animatable-impl.h",
        "animatable.h",
        "debug.h",
        "folder_resource_loader.h",
        "interpolate.h",
        "render_system.h",
        "time.h",
    ],

    "target_conditions": [
        [
            "OS == 'mac' or OS == 'ios'",
            {
                "sources": [
                    "bundle_resource_loader_ns.h",
                ],
            }
        ],
    ],
}