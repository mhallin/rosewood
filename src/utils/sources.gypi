{
    "sources": [
        "animatable.cc",
        "debug.cc",
        "folder_resource_loader.cc",
        "render_system.cc",
        "time.cc",
    ],

    "target_conditions": [
        [
            "OS == 'mac' or OS == 'ios'",
            {
                "sources": [
                    "bundle_resource_loader_ns.mm",
                ],
            }
        ],
    ],
}