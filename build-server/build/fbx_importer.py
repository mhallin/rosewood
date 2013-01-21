import math

from itertools import chain


class ParseBuffer(object):
    def __init__(self, f):
        self._data = f.read()
        self._pos = 0
        self._size = len(self._data)

    def read(self):
        self._pos += 1
        p = self._pos
        if p >= self._size:
            return ''
        return self._data[p]

    def peek(self):
        p = self._pos
        if p >= self._size:
            return ''
        return self._data[p]

    def read_back(self):
        self._pos -= 1

    def read_up_to(self, *chars):
        s = self.peek()
        if not s:
            return ''

        while True:
            c = self.read()
            if c and c not in chars:
                s += c
            else:
                return s

    def read_while(self, *chars):
        s = self.peek()
        if not s:
            return ''

        while True:
            c = self.read()
            if c in chars:
                s += c
            else:
                return s


class FBXObject(object):
    def __init__(self, key, properties, sub_objects):
        self.key = key
        self.properties = properties
        self.sub_objects = sub_objects

    def __repr__(self):
        sub_objs_str = ','.join((repr(so) for so in self.sub_objects)
                                if self.sub_objects else [])
        return '<FBXObject "{}": {}, [{}]>'.format(self.key,
                                                   repr(self.properties),
                                                   sub_objs_str)

    def find_object(self, key):
        for obj in self.sub_objects or []:
            if obj.key == key:
                return obj

    def find_objects(self, key):
        for obj in self.sub_objects or []:
            if obj.key == key:
                yield obj

    def get_property(self, key):
        obj = self.find_object(key)
        return obj.properties[0]


def parse_number(b):
    s = b.read_while(*'-0123456789')
    if not s:
        raise Exception('eof while reading number')
    if b.peek() == '.':
        b.read()
        s += '.' + b.read_while(*'0123456789')
        return float(s)
    return int(s)


def parse_string(b):
    s = ''
    b.read()
    p = b.peek()
    while p != '"':
        s += p
        b.read()
        p = b.peek()
    b.read()
    return s


def parse_object(b):
    # Read property key
    skip_whitespace(b)
    key = b.read_up_to(':')

    # Skip ':'
    b.read()

    # Skip whitespace
    skip_whitespace(b)

    properties = []
    sub_objs = None

    p = b.peek()

    while p not in ('{'):
        if p in '-0123456789':
            properties.append(parse_number(b))
        elif p == '"':
            properties.append(parse_string(b))
        elif p in ('W', 'Y', 'L'):
            properties.append(b.peek())
            b.read()

        skip_whitespace(b)
        if b.peek() == ',':
            b.read()
            skip_whitespace(b)
        else:
            break

        p = b.peek()

    # If there's a {, skip it and read objects until the matching }
    if b.peek() == '{':
        sub_objs = []
        b.read()
        skip_whitespace(b)
        while b.peek() != '}':
            sub_objs.append(parse_object(b))
            skip_whitespace(b)
        b.read()

    return FBXObject(key, properties, sub_objs)


def skip_whitespace(b):
    ws_chars = (' ', '\t', '\n')

    c = b.peek()

    while True:
        if not c:
            return False
        if c == ';':
            l = b.read_up_to('\n')
            if not l:
                return False
        elif c not in ws_chars:
            return True

        c = b.read()


def parse_fbx(f):
    b = ParseBuffer(f)
    toplevel_objects = {}
    while skip_whitespace(b):
        obj = parse_object(b)
        toplevel_objects[obj.key] = obj

    return toplevel_objects


def vertex_index(idx):
    return idx if idx >= 0 else ~idx


def make_vertices(verts, size):
    vs = []
    v = []

    for i, p in enumerate(verts):
        v.append(p)
        if len(v) == size:
            vs.append(v)
            v = []

    return vs


def tesselate_poly(poly):
    if len(poly) == 3:
        return [poly]

    if len(poly) == 4:
        t1 = (poly[0], poly[1], poly[2])
        t2 = (poly[0], poly[2], poly[3])
        return [t1, t2]


def tesselate_poly_list(polys):
    return list(chain(*map(tesselate_poly, polys)))


def flatten(l):
    r = []
    for i in l:
        if hasattr(i, '__iter__'):
            r += flatten(i)
        else:
            r.append(i)

    return r


def vec_cross(v1, v2):
    x1, y1, z1 = v1
    x2, y2, z2 = v2

    return (y1 * z2 - z1 * y2,
            z1 * x2 - x1 * z2,
            x1 * y2 - y1 * x2)


def vec_normalized(v):
    x, y, z = v
    norm = math.sqrt(x*x + y*y + z*z)

    if norm != 0:
        return (x/norm, y/norm, z/norm)
    return v


def triangle_normal(tri):
    (x1, y1, z1), (x2, y2, z2), (x3, y3, z3) = tri

    v1 = (x2 - x1, y2 - y1, z2 - z1)
    v2 = (x3 - x1, y3 - y1, z3 - z1)
    norm = vec_cross(v1, v2)

    return vec_normalized(norm)


def vec_len(v):
    return math.sqrt(sum(c*c for c in v))


def angle_between(v1, v2):
    dot = sum(v1p * v2p for v1p, v2p in zip(v1, v2))
    if dot > 1:
        dot = 1

    return math.acos(dot)


def lerp(v1, v2, mix):
    return [v1c + (v2c - v1c) * mix for v1c, v2c in zip(v1, v2)]


class FBXMesh(object):
    def __init__(self, name, vertices, vertex_indices):
        self.name = name
        self.vertices = make_vertices(vertices, 3)
        self.vertex_indices = vertex_indices
        self.normals = {}
        self.uvs = {}

        self._construct_polygon_indices()

    def add_normal_object(self, normal_object):
        name = normal_object.get_property('Name')
        self.normals[name] = self._construct_vert_list(normal_object,
                                                       'Normals', 3)

    def add_uv_object(self, uv_object):
        name = uv_object.get_property('Name')
        self.uvs[name] = self._construct_vert_list(uv_object, 'UV', 2)

    def map_polygon_indices(self, fn):
        def map_inner(poly):
            return map(fn, poly)

        return map(map_inner, self.polygon_indices)

    def tesselate(self):
        self.polygon_indices = tesselate_poly_list(self.polygon_indices)

        for key, normals in self.normals.iteritems():
            self.normals[key] = tesselate_poly_list(self.normals[key])

        for key, uvs in self.uvs.iteritems():
            self.uvs[key] = tesselate_poly_list(self.uvs[key])

    def construct_polygons(self):
        def vertex_by_index(idx):
            return self.vertices[idx]

        self.polygons = self.map_polygon_indices(vertex_by_index)

    def make_flattened_dict(self):
        return {'vertices': flatten(self.polygons),
                'normals': dict((k, flatten(v))
                                for k, v in self.normals.iteritems()),
                'uvs': dict((k, flatten(v))
                            for k, v in self.uvs.iteritems())
                }

    def recalculate_normals(self, key=None):
        if key is None:
            key = self.normals.keys()[0]

        normals = []

        for pi in range(len(self.polygons)):
            tri = self.polygons[pi]
            assert len(tri) == 3

            tri_norm = [triangle_normal(tri), ] * 3
            normals.append(tri_norm)

        self.normals[key] = normals

    def smooth_normals(self, threshold, key=None):
        if key is None:
            normals = self.normals.values()[0]
        else:
            normals = self.normals[key]

        threshold *= math.pi / 180.0

        for pi, tri in enumerate(self.polygons):
            for vi in range(len(tri)):
                src_norm = normals[pi][vi]
                for shared_pi, shared_vi in self._shared_edges(pi, vi):
                    dst_norm = normals[shared_pi][shared_vi]

                    angle = angle_between(src_norm, dst_norm)

                    if angle <= threshold:
                        norm = lerp(src_norm, dst_norm, 0.5)
                        normals[pi][vi] = norm
                        normals[shared_pi][shared_vi] = norm


    def _shared_edges(self, pi, vi):
        v = self.polygons[pi][vi]
        shared = []

        for poly, tri in enumerate(self.polygons):
            for i, vert in enumerate(tri):
                if vert == v and (pi, vi) != (poly, i):
                    shared.append((poly, i))

        return shared


    def _construct_polygon_indices(self):
        polys = []
        poly = []
        for idx in self.vertex_indices:
            poly.append(vertex_index(idx))
            if idx < 0:
                polys.append(poly)
                poly = []

        self.polygon_indices = polys

    def _construct_vert_list(self, obj, kind, size):
        map_type = obj.get_property('MappingInformationType')
        ref_info_type = obj.get_property('ReferenceInformationType')

        if map_type == 'ByVertice':
            vertices = make_vertices(obj.find_object(kind).properties, size)
            return self.map_polygon_indices(lambda i: vertices[i])

        if map_type == 'ByPolygonVertex':
            vertices = make_vertices(obj.find_object(kind).properties, size)
            indices = obj.find_object(kind + 'Index').properties
            index_dict = dict((vi, i) for i, vi in enumerate(indices))

            result = []
            n = 0
            for p in self.polygon_indices:
                r = []
                for i in p:
                    r.append(vertices[index_dict[n]])
                    n += 1
                result.append(r)

            return result


def construct_mesh(mesh_object):
    name = mesh_object.properties[1]

    vertices_object = mesh_object.find_object('Vertices')
    vertices = vertices_object.properties

    vertex_indices_object = mesh_object.find_object('PolygonVertexIndex')
    vertex_indices = vertex_indices_object.properties

    mesh = FBXMesh(name, vertices, vertex_indices)

    for normal_object in mesh_object.find_objects('LayerElementNormal'):
        mesh.add_normal_object(normal_object)

    for uv_object in mesh_object.find_objects('LayerElementUV'):
        mesh.add_uv_object(uv_object)

    mesh.tesselate()
    mesh.construct_polygons()

    return mesh


if __name__ == '__main__':
    import sys
    import pprint
    args = sys.argv[1:]

    if len(args) == 0:
        print 'Usage: fbx_importer.py dump <filename>'
        sys.exit(1)

    if args[0] == 'dump-mesh':
        with file(args[1]) as fbxfile:
            objs = parse_fbx(fbxfile)['Objects']
            mesh_object = [o
                           for o in objs.find_objects('Model')
                           if o.properties[1] == 'Mesh'][0]

            mesh = construct_mesh(mesh_object)
            print 'recalculating normals'
            mesh.recalculate_normals()
            mesh.smooth_normals(40)
            for pi in range(len(mesh.polygons)):
                verts = mesh.polygons[pi]
                norms = mesh.normals[''][pi]
                uvs = mesh.uvs.values()[0][pi]

                for i in range(len(verts)):
                    print '{:35}{:40}{}'.format(verts[i], uvs[i], norms[i])

from io import StringIO
from unittest import TestCase


class FBXImporterTest(TestCase):
    def test_parse_empty_object(self):
        obj = self._parse_obj_str(u'Key:  {\n}')

        self.assertEqual(obj.key, 'Key')
        self.assertEqual(len(obj.sub_objects), 0)
        self.assertEqual(obj.properties, [])

    def test_parse_nullary_property(self):
        obj = self._parse_obj_str(u'Key:\n')

        self.assertEqual(obj.key, 'Key')
        self.assertEqual(obj.sub_objects, None)
        self.assertEqual(obj.properties, [])

    def test_parse_object_with_subproperty(self):
        obj = self._parse_obj_str(u'Parent:  {\n  Child:\n}')

        self.assertEqual(obj.key, 'Parent')
        self.assertEqual(len(obj.sub_objects), 1)
        self.assertEqual(obj.sub_objects[0].key, 'Child')
        self.assertEqual(obj.sub_objects[0].sub_objects, None)
        self.assertEqual(obj.properties, [])

    def test_parse_empty_object_with_int_property(self):
        obj = self._parse_obj_str(u'Key: 123 {\n}')

        self.assertEqual(obj.key, 'Key')
        self.assertEqual(obj.sub_objects, [])
        self.assertEqual(obj.properties, [123])

    def test_parse_empty_object_with_string_property(self):
        obj = self._parse_obj_str(u'Key: "prop" {\n}')

        self.assertEqual(obj.key, 'Key')
        self.assertEqual(obj.sub_objects, [])
        self.assertEqual(obj.properties, ['prop'])

    def test_parse_property(self):
        obj = self._parse_obj_str(u'Key: 123, 123.5, -123, W, "prop"\n')

        self.assertEqual(obj.key, 'Key')
        self.assertEqual(obj.sub_objects, None)
        self.assertEqual(obj.properties, [123, 123.5, -123, 'W', 'prop'])

    def test_parse_multiline_property(self):
        obj = self._parse_obj_str(u'Key: 123,456\n,789,123\n,456,789\n')

        self.assertEqual(obj.key, 'Key')
        self.assertEqual(obj.properties, [123, 456, 789, 123, 456, 789])

    def test_parse_property_with_continuation(self):
        obj = self._parse_obj_str(u'Key1: 123\nKey2: 456\n')

        self.assertEqual(obj.key, 'Key1')
        self.assertEqual(obj.properties, [123])
        self.assertEqual(obj.sub_objects, None)

    def test_parse_property_on_newline(self):
        obj = self._parse_obj_str(u'Key:\n123\n')

        self.assertEqual(obj.key, 'Key')
        self.assertEqual(obj.properties, [123])
        self.assertEqual(obj.sub_objects, None)

    def test_parse_two_properties(self):
        with StringIO(u'  Key1: 123\n  Key2: 456\n') as f:
            b = ParseBuffer(f)
            obj1 = parse_object(b)
            obj2 = parse_object(b)

            self.assertEqual(obj1.key, 'Key1')
            self.assertEqual(obj1.sub_objects, None)
            self.assertEqual(obj1.properties, [123])

            self.assertEqual(obj2.key, 'Key2')
            self.assertEqual(obj2.sub_objects, None)
            self.assertEqual(obj2.properties, [456])

    def test_parse_two_objects(self):
        with StringIO(u'  Key1: 123 {\n}\n  Key2: 456 {\n}\n') as f:
            b = ParseBuffer(f)
            obj1 = parse_object(b)
            obj2 = parse_object(b)

            self.assertEqual(obj1.key, 'Key1')
            self.assertEqual(obj1.sub_objects, [])
            self.assertEqual(obj1.properties, [123])

            self.assertEqual(obj2.key, 'Key2')
            self.assertEqual(obj2.sub_objects, [])
            self.assertEqual(obj2.properties, [456])

    def _parse_obj_str(self, s):
        with StringIO(s) as f:
            return parse_object(ParseBuffer(f))
