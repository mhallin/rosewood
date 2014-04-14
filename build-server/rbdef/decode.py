# -*- encoding: utf-8 -*-

'''
RBDEF decoder.

With the exception of unicode decoding, ``dump`` and ``dumps`` are the
inverses of ``load`` and ``loads`` from the encoding module.

>>> from encode import dumps

>>> loads(dumps(None))

>>> loads(dumps(True))
True
>>> loads(dumps(False))
False

>>> loads(dumps(123))
123
>>> loads(dumps(2**54))
18014398509481984

>>> loads(dumps(5.5))
5.5
>>> loads(dumps(1.0322368742309358))
1.0322368742309358

>>> loads(dumps('test\\x00more')) == b'test\\x00more'
True

>>> loads(dumps([1, 2, 3]))
[1, 2, 3]

>>> result = loads(dumps({'x': 1, 'y': 2, 'z': 3}))
>>> result[b'x'], result[b'y'], result[b'z']
(1, 2, 3)

'''

from __future__ import unicode_literals

import sys

from io import BytesIO
from struct import Struct

PY3 = sys.version_info >= (3, 0)


def _unpack_read(fmt, fp):
    struct = Struct(fmt)
    buf = fp.read(struct.size)

    return struct.unpack(buf)


def _load_uint(is_large, fp):
    return _unpack_read(b'>Q' if is_large else b'>I', fp)[0]


def _load_int(dsc, fp):
    return _unpack_read(b'>i' if dsc == b'i' else b'>q', fp)[0]


def _load_float(dsc, fp):
    return _unpack_read(b'>f' if dsc == b'p' else b'>d', fp)[0]


def _load_str(dsc, fp):
    size = _load_uint(dsc == 'S', fp)
    return fp.read(size)


def _load_array(dsc, fp):
    size = _load_uint(dsc == 'A', fp)
    return [load(fp) for _ in range(size)]


def _load_dict(dsc, fp):
    size = _load_uint(dsc == 'D', fp)
    return dict((load(fp), load(fp)) for _ in range(size))


def load(fp):
    '''Read a RBDEF object from the file-like object ``fp``.'''
    b = fp.read(1)

    if b == b'n':
        return None

    if b == b'f':
        return False

    if b == b't':
        return True

    if b in (b'i', b'I'):
        return _load_int(b, fp)

    if b in (b'p', b'P'):
        return _load_float(b, fp)

    if b in (b's', b'S'):
        return _load_str(b, fp)

    if b in (b'a', b'A'):
        return _load_array(b, fp)

    if b in (b'd', b'D'):
        return _load_dict(b, fp)

    raise ValueError('Unknown byte discriminator "%s"' % repr(b))


def loads(b):
    '''Read a RBDEF object from the byte string ``b``.'''
    of = BytesIO(b)
    try:
        return load(of)
    finally:
        of.close()


__all__ = ['load', 'loads']
