****************************************
The Rosewood Binary Data Exchange Format
****************************************

The Rosewood Binary Data Exchange Format, RBDEF for short, is a 1-to-1 mapping of JSON to
a binary scheme.

Doesn't That Already Exist?
===========================

It does. However, Rosewood needs something that can just be dropped into a project and built
as a part of the standard build procedure. This means no pre-generated config.h files, which
eliminates most Autotools-based libraries.

Rosewood also needs something that can run on Emscripten and ARM without fuss. As a minor
point, Rosewood also only needs a reader written in C++ since the build tools that will
generate these files is written in Python.

Thus, RBDEF.

Plus, it isn't *that* hard to do.

Encoding
========

Primitive values::

    Byte  | Value
    ======|========
    'n'   | NULL
    'f'   | false
    't'   | true

Numbers::

    Byte  | Followed by           | Value
    ======|=======================|==========================================
    'i'   | 4 bytes big endian    | Signed 32 bit integer
    'I'   | 8 bytes big endian    | Signed 64 bit integer
    'p'   | 4 bytes big endian    | IEEE 754 single precision
    'P'   | 8 bytes big endian    | IEEE 754 double precision

Raw string data. Encoding is not specified, as this is most commonly used by Rosewood to
transfer binary data. For non-binary data, however, this *should* be treated as UTF-8::

    Byte  | Followed by           | Value
    ======|=======================|==========================================
    's'   | 4 bytes big endian    | <4 GiB raw string data
    'S'   | 8 bytes big endian    | <16 EiB raw string data

Arrays::

    Byte  | Followed by           | Value
    ======|=======================|==========================================
    'a'   | 4 bytes big endian    | Array with <2^32 items
    'A'   | 8 bytes big endian    | Array with <2^64 items

Dictionaries. Keys are assumed to be of string type, but they are still encoded as
regular objects::

    Byte  | Followed by           | Value
    ======|=======================|==========================================
    'd'   | 4 bytes big endian    | Dictionary with <2^32 key/value pairs
    'D'   | 8 bytes big endian    | Dictionary with <2^64 key/value pairs

And that's it! Simple, really.
