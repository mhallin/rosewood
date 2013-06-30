# Rosewood — Interactive CES-based 3D renderer

### System includes:

* Renderer
* Build server
* Utilities to load texture/mesh data

### Renderer includes:

* Entity manager with memory efficient component storage
* Transform hierarchy
* Texture loading
* Shader parameters
* Arbitrary mesh data (in addition to vertex/normal/texcoord)
* Zeroconf/Bonjour service announcement

### Build server includes:

* FBX loader support
* Zeroconf/Bonjour service discovery
* Automatic builds when files reload
* Automatic uploads to running renderers

### Tested on:

* iOS (does not compile until new SSE math has been ported)
* OS X
* FreeBSD with X11

### Dependencies:

* OpenGL 3.2 (or ES 2) implementation (GLX or Cocoa)
* libev
* msgpack
* Google C++ testing
* Image loader (libpng or Cocoa)
* Window system (X11 or Cocoa)

## How to build

The project uses and includes waf. Configure for different platforms:

    ./waf configure --modules=libpng,x11 # On *nix
    ./waf configure --modules=osx,ns # On OS X
    ./waf configure --modules=ios,ns # On iOS
    
Build it:

    ./waf build_debug # Optimizations on, debug info on
    ./waf build_release # Optimizations on, debug info off
    ./waf build_slow_debug # Optimizations off, debug info on, check every GL call
    
Test it:

    ./build/{debug,release,slow_debug}/rosewood_tests
     
Run it:

    ./build/{debug,release,slow_debug}/rwexample
    
On OS X/iOS you can use the Xcode project to get an .app.
