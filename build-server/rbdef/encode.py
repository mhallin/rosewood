# -*- encoding: utf-8 -*-

from __future__ import unicode_literals

import struct
import sys

from io import BytesIO

PY3 = sys.version_info >= (3, 0)


def _dump_uint(prefix_chars, obj, fp):
    if obj < 2**32:
        fp.write(prefix_chars[0])
        fp.write(struct.pack(b'>I', obj))
    else:
        fp.write(prefix_chars[1])
        fp.write(struct.pack(b'>Q', obj))


def dump_none(fp):
    '''Write a null

    >>> dumps(None) == b'n'
    True
    '''

    fp.write(b'n')


def dump_bool(obj, fp):
    '''Write a boolean

    >>> dumps(False) == b'f'
    True
    >>> dumps(True) == b't'
    True
    '''

    fp.write(b't' if obj else b'f')


def dump_int(obj, fp):
    '''Write an integer

    >>> dumps(0x0a0b0c0d) == b'i\\x0a\\x0b\\x0c\\x0d'
    True
    >>> dumps(0x0a0b0c0d0e0f0001) == b'I\\x0a\\x0b\\x0c\\x0d\\x0e\\x0f\\x00\\x01'
    True
    '''

    if obj < 2**31 and obj > -2**31:
        fp.write(b'i')
        fp.write(struct.pack(b'>i', obj))
    elif obj < 2**63 and obj > -2**63:
        fp.write(b'I')
        fp.write(struct.pack(b'>q', obj))
    else:
        raise ValueError('Int out of range: %s' % obj)


def dump_float(obj, fp):
    '''Write a float

    >>> dumps(5.5) == b'p\\x40\\xb0\\x00\\x00'
    True
    >>> dumps(1.03223687423093579695887456182) == b'P\\x3F\\xF0\\x84\\x0A\\xD0\\x08\\xC1\\x10'
    True
    '''
    obj_packed = struct.pack(b'>f', obj)
    obj_unpacked = struct.unpack(b'>f', obj_packed)[0]

    if obj_unpacked == obj:
        fp.write(struct.pack(b'>cf', b'p', obj))
    else:
        fp.write(struct.pack(b'>cd', b'P', obj))


def dump_str(obj, fp):
    '''Write a string

    >>> dumps('test\\x00more') == b's\\x00\\x00\\x00\\x09test\\x00more'
    True
    '''

    _dump_uint((b's', b'S'), len(obj), fp)
    fp.write(obj)


def dump_unicode(obj, fp):
    '''Write a unicode string as UTF-8

    >>> dumps('åäö') == b's\\x00\\x00\\x00\\x06\\xc3\\xa5\\xc3\\xa4\\xc3\\xb6'
    True
    '''

    dump_str(obj.encode('utf-8'), fp)


def dump_array(obj, fp):
    '''Write an array of objects

    >>> dumps([1, 2, 3]) == (b'a\\x00\\x00\\x00\\x03'
    ...                      b'i\\x00\\x00\\x00\\x01'
    ...                      b'i\\x00\\x00\\x00\\x02'
    ...                      b'i\\x00\\x00\\x00\\x03')
    True
    '''

    _dump_uint((b'a', b'A'), len(obj), fp)
    for elem in obj:
        dump(elem, fp)


def dump_dict(obj, fp):
    '''Write a dictionary of string/object pairs

    >>> from collections import OrderedDict
    >>> dumps(OrderedDict([('x', 1), ('y', 2), ('z', 3)])) == \\
    ...     (b'd\\x00\\x00\\x00\\x03'
    ...      b's\\x00\\x00\\x00\\x01xi\\x00\\x00\\x00\\x01'
    ...      b's\\x00\\x00\\x00\\x01yi\\x00\\x00\\x00\\x02'
    ...      b's\\x00\\x00\\x00\\x01zi\\x00\\x00\\x00\\x03')
    True
    '''

    _dump_uint((b'd', b'D'), len(obj), fp)
    for key, value in obj.items():
        dump(key, fp)
        dump(value, fp)


def dump(obj, fp):
    '''Serialize ``obj`` as a RBDEF object to ``fp``.'''

    if obj is None:
        dump_none(fp)
    elif isinstance(obj, bool):
        dump_bool(obj, fp)
    elif isinstance(obj, int):
        dump_int(obj, fp)
    elif isinstance(obj, float):
        dump_float(obj, fp)
    elif not PY3 and isinstance(obj, str):
        dump_str(obj, fp)
    elif not PY3 and isinstance(obj, unicode):
        dump_unicode(obj, fp)
    elif PY3 and isinstance(obj, bytes):
        dump_str(obj, fp)
    elif PY3 and isinstance(obj, str):
        dump_unicode(obj, fp)
    elif isinstance(obj, list) or isinstance(obj, tuple):
        dump_array(obj, fp)
    elif isinstance(obj, dict):
        dump_dict(obj, fp)
    else:
        raise ValueError('Can not serialize %s' % str(obj))


def dumps(obj):
    '''Serialize ``obj`` as a RBDEF object to a str/bytes object, depending
    on Python 2/3.'''

    of = BytesIO()
    try:
        dump(obj, of)
        return of.getvalue()
    finally:
        of.close()


__all__ = ['dump', 'dumps']
