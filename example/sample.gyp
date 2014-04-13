{
    "includes": [
        "../common.gypi",
    ],

    "targets": [
        {
            "target_name": "rw_sample",
            "type": "executable",
            "mac_bundle": 1,

            "sources": [
                "sample_app.cc",
            ],

            "actions": [
                {
                    "action_name": "Run rosewood build tasks",
                    "inputs": [],
                    "outputs": [
                        "asset-build/rw_build.cache",
                        "__always_run_build_graph__"
                    ],
                    "action": [
                        "python",
                        "../build-server/build/build_graph.py",
                        "build"
                    ],
                },
            ],

            "target_conditions": [
                [
                    "OS == 'mac'",
                    {
                        "sources": [
                            "osx/RWOSXAppDelegate.mm",
                            "osx/main.m",
                            "osx/RWSample-Prefix.pch",
                        ],

                        "libraries": [
                            "$(SDKROOT)/System/Library/Frameworks/Foundation.framework",
                            "$(SDKROOT)/System/Library/Frameworks/Cocoa.framework",
                            "$(SDKROOT)/System/Library/Frameworks/OpenGL.framework",
                        ],

                        "mac_bundle_resources": [
                            "osx/en.lproj/Credits.rtf",
                            "osx/en.lproj/InfoPlist.strings",
                            "osx/en.lproj/MainMenu.xib",
                        ],

                        "xcode_settings": {
                            "INFOPLIST_FILE": "osx/RWSample-Info.plist",
                        },
                    }
                ],
                [
                    "OS == 'ios'",
                    {
                        "sources": [
                            "ios/RWiOSAppDelegate.mm",
                            "ios/main.m",
                            "ios/RWSample-Prefix.pch",
                        ],

                        "libraries": [
                            "$(SDKROOT)/System/Library/Frameworks/Foundation.framework",
                            "$(SDKROOT)/System/Library/Frameworks/UIKit.framework",
                            "$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework",
                            "$(SDKROOT)/System/Library/Frameworks/OpenGLES.framework",
                            "$(SDKROOT)/System/Library/Frameworks/GLKit.framework",
                        ],

                        "mac_bundle_resources": [
                            "ios/en.lproj/InfoPlist.strings",
                            "ios/Default-568h@2x.png",
                            "ios/Default.png",
                            "ios/Default@2x.png",
                        ],

                        "xcode_settings": {
                            "INFOPLIST_FILE": "ios/RWSample-Info.plist",
                        },
                    }
                ],
                [
                    "OS == 'ios' or OS == 'mac'",
                    {
                        "actions": [
                            {
                                "action_name": "Copy asset-build to app bundle",
                                "inputs": [
                                    "asset-build/rw_build.cache",
                                ],
                                "outputs": [
                                    "<(PRODUCT_DIR)/${UNLOCALIZED_RESOURCES_FOLDER_PATH}/asset-build/rw_build.cache",
                                ],
                                "action": [
                                    "rsync", "--recursive", "--delete", "--update",
                                    "asset-build",
                                    "${BUILT_PRODUCTS_DIR}/${UNLOCALIZED_RESOURCES_FOLDER_PATH}",
                                ],
                            },
                        ],
                    }
                ]
            ],

            "dependencies": [
                "../src/engine.gyp:rw_core",
                "../src/engine.gyp:rw_math",
                "../src/engine.gyp:rw_graphics",
                "../src/engine.gyp:rw_platform",
                "../src/engine.gyp:rw_utils",
            ],
        },
    ],
}