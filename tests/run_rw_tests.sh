#!/bin/sh

# This file should be copied to out/$CONFIGURATION/rw_tests and run there. It assumes that
# rw_tests.js exists in the same folder, and emscripten is present in the ../../deps folder.

exec ../../deps/emsdk_portable/node/0.10.18_64bit/bin/node rw_tests.js
