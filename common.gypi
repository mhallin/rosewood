{
    "target_defaults": {
        "xcode_settings": {

            "GCC_C_LANGUAGE_STANDARD": "c11",
            "CLANG_CXX_LANGUAGE_STANDARD": "c++0x",
            "CLANG_CXX_LIBRARY": "libc++",

            "ALWAYS_SEARCH_USER_PATHS": "NO",
            "USE_HEADERMAP": "NO",

            "GCC_ENABLE_CPP_RTTI": "NO",
            "CLANG_ENABLE_OBJC_ARC": "YES",
            "GCC_DYNAMIC_NO_PIC": "NO",
            "GCC_INLINES_ARE_PRIVATE_EXTERN": "NO",
            "GCC_SYMBOLS_PRIVATE_EXTERN": "NO",

            "CLANG_WARN_BOOL_CONVERSION": "YES",
            "CLANG_WARN_CONSTANT_CONVERSION": "YES",
            "CLANG_WARN_DIRECT_OBJC_ISA_USAGE": "YES_ERROR",
            "CLANG_WARN_DOCUMENTATION_COMMENTS": "YES",
            "CLANG_WARN_EMPTY_BODY": "YES",
            "CLANG_WARN_ENUM_CONVERSION": "YES",
            "CLANG_WARN_INT_CONVERSION": "YES",
            "GCC_TREAT_IMPLICIT_FUNCTION_DECLARATIONS_AS_ERRORS": "YES",
            "GCC_WARN_64_TO_32_BIT_CONVERSION": "YES",
            "GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS": "YES",
            "GCC_WARN_ABOUT_MISSING_NEWLINE": "YES",
            "GCC_WARN_ABOUT_MISSING_PROTOTYPES": "YES",
            "GCC_WARN_ABOUT_RETURN_TYPE": "YES_ERROR",
            "GCC_WARN_UNINITIALIZED_AUTOS": "YES",
            "GCC_WARN_UNUSED_FUNCTION": "YES",
            "GCC_WARN_UNUSED_LABEL": "YES",
            "GCC_WARN_UNUSED_PARAMETER": "YES",
            "GCC_WARN_UNUSED_VARIABLE": "YES",
            "GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS": "YES",
            "CLANG_WARN_OBJC_ROOT_CLASS": "YES_ERROR",
            "CLANG_WARN__DUPLICATE_METHOD_MATCH": "YES",
            "GCC_WARN_UNDECLARED_SELECTOR": "YES",

            "WARNING_CFLAGS": [
                "-Wno-gcc-compat",
            ],
        },

        "target_conditions": [
            [
                "OS == 'ios'",
                {
                    "xcode_settings": {
                        "SDKROOT": "iphoneos",
                    },
                }
            ],
        ],

        "configurations": {
            "debug": {
                "xcode_settings": {
                    "GCC_OPTIMIZATION_LEVEL": "0",
                },
            },

            "debug_slowrender": {
                "xcode_settings": {
                    "GCC_OPTIMIZATION_LEVEL": "0",
                },
            },

            "release": {
                "xcode_settings": {
                    "GCC_OPTIMIZATION_LEVEL": "3",
                },

                "target_conditions": [
                    [
                        "_type == 'executable'",
                        {
                            "xcode_settings": {
                                "LLVM_LTO": "YES",
                                "OTHER_LDFLAGS": [
                                    "-flto",
                                ],
                            },
                        },
                    ],
                ],
            },
        },
    },
}