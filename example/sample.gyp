{
    "includes": [
        "../common.gypi",
    ],

    "targets": [
        {
            "target_name": "rw_sample",
            "type": "executable",

            "sources": [
                "sample_app.cc",
            ],

            "actions": [
                {
                    "action_name": "Run rosewood build tasks",
                    "inputs": [],
                    "outputs": [
                        "asset-build/rw_build_cache.rbdef",
                        "__always_run_build_graph__"
                    ],
                    "action": [
                        "../build-server/build.py",
                        "build"
                    ],
                },
            ],

            "target_conditions": [
                [
                    "OS == 'mac'",
                    {
                        "mac_bundle": 1,

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
                        "mac_bundle": 1,

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
                    "(OS == 'ios' or OS == 'mac') and GENERATOR == 'ninja'",
                    {
                        "actions": [
                            {
                                "action_name": "Copy asset-build to app bundle",
                                "inputs": [
                                    "asset-build/rw_build_cache.rbdef",
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
                ],
                [
                    "(OS == 'ios' or OS == 'mac') and GENERATOR == 'xcode'",
                    {
                        "postbuilds": [
                            {
                                "postbuild_name": "Copy asset-build to app bundle",
                                "action": [
                                    "rsync", "--recursive", "--delete", "--update",
                                    "asset-build",
                                    "${BUILT_PRODUCTS_DIR}/${UNLOCALIZED_RESOURCES_FOLDER_PATH}",
                                ],
                            },
                        ],
                    }
                ],
                [
                    "OS == 'emscripten'",
                    {
                        "product_name": "rw_sample.html",

                        "sources": [
                            "emscripten/main.cc",
                        ],

                        "xcode_settings": {
                            "OTHER_LDFLAGS": [
                                "-s STB_IMAGE=1",
                                "-s WARN_ON_UNDEFINED_SYMBOLS=1",
                                "--embed-file", "../../example/asset-build@/",
                            ],
                        },
                    }
                ],
            ],

            "dependencies": [
                "../engine/engine.gyp:rw_core",
                "../engine/engine.gyp:rw_data_format",
                "../engine/engine.gyp:rw_math",
                "../engine/engine.gyp:rw_graphics",
                "../engine/engine.gyp:rw_platform",
                "../engine/engine.gyp:rw_utils",
            ],
        },
    ],
}