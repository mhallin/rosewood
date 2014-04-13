********
Rosewood
********

.. image:: https://travis-ci.org/mhallin/rosewood.svg?branch=master
   :target: https://travis-ci.org/mhallin/rosewood

An interactive CES-based 3D-renderer. Written to be platform independent, builds on GLX/X11, OS X,
and iOS.

Systems Included
================

* Renderer
* Build server
* Utilities to load texture/mesh data

Renderer Features
=================

* Modular design
* Entity manager with memory efficient component storage
* Transform hierarchy
* Texture loading
* Shader parameters
* Arbitrary mesh data (in addition to vertex/normal/texcoord)
* Zeroconf/Bonjour service announcement

Build System Features
=====================

* FBX loader support
* Zeroconf/Bonjour service discovery
* Automatic builds when files reload
* Automatic uploads to running renderers

Tested On
=========

* iOS (does not compile until new SSE math has been ported)
* OS X
* FreeBSD with X11

It is mainly developed on OS X, which means that it is probably broken on all other platforms
right now.

Dependencies
============

* OpenGL 3.2 (or ES 2) implementation (GLX or Cocoa)
* libev
* msgpack
* Google C++ testing
* Image loader (libpng or Cocoa)
* Window system (X11 or Cocoa)

Dependencies are pulled in either via submodules or via the ``rwconfig.py`` script.

How To Build
============

The project is using Google's Gyp, which can generate project files for your build system
of choice. The ``rwconfig.py`` script is used for dependency management/building, and it builds
for the Ninja build system by default::

    python rwconfig.py    # Configure it
    cd out/debug          # Go to the configuration folder
    ninja                 # Build it

Now you have the sample application and unit tests built::

    ./rw_tests            # Run unit tests
    open rw_sample.app    # Open the sample application
    
If you want an Xcode project, you can use gyp directly::

    python rwconfig.py    # Make sure dependencies are in order
    # Generates rosewood.xcodeproj in the current directory
    ./deps/gyp/gyp --depth=. -DOS=mac -f xcode example/sample.gyp rosewood.gyp

What Works?
===========

A few things:

* The asset pipeline and build system
* Texture loading, shader compilation, rendering

What Doesn't Work?
==================

A lot of things.

* There's a physics folder in src. I haven't pulled in Bullet in the configure script yet
* The remote-control isn't compiling at the moment

How Do I Use It?
================

That's not so easy. But you can try the following:

* Copy the example project to your own repository
* Add this repository as a submodule
* Fix all pathnames in the example.gyp so they refer to your submodule's common.gypi
* Start hacking away on the ``sample_app.cc``