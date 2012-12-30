class OBJMesh(object):
    def __init__(self, vertices, texcoords, normals, tris, name):
        self.vertices = vertices
        self.texcoords = texcoords
        self.normals = normals
        self.tris = tris
        self.name = name

    def make_flattened_dict(self):
        vertices = []
        texcoords = []
        normals = []

        for triangle in self.tris:
            for (vi, ti, ni) in triangle:
                vertices += self.vertices[vi - 1]
                texcoords += self.texcoords[ti - 1]
                normals += self.normals[ni - 1]

        return {'vertices': vertices,
                'texcoords': {'': texcoords},
                'normals': {'': normals}}

def parse_obj(f):
    vertices = []
    texcoords = []
    normals = []
    tris = []
    name = '(unknown)'

    for line in (l.strip() for l in f.readlines()):
        items = line.split(' ')

        if items[0] == 'v':
            vertices.append(tuple(float(i) for i in items[1:]))
        elif items[0] == 'vt':
            texcoords.append(tuple(float(i) for i in items[1:]))
        elif items[0] == 'vn':
            normals.append(tuple(float(i) for i in items[1:]))
        elif items[0] == 'f':
            tris.append(tuple(tuple(int(n) for n in i.split('/')) for i in items[1:]))
        elif items[0] == 'g':
            name = items[1]

    return OBJMesh(vertices, texcoords, normals, tris, name)

if __name__ == '__main__':
    import sys
    import pprint
    args = sys.argv[1:]

    if len(args) == 0:
        print 'Usage: obj_importer.py dump <filename>'
        sys.exit(1)

    if args[0] == 'dump-mesh':
        with file(args[1]) as fbxfile:
            mesh = parse_obj(fbxfile)

#        print '- Mesh:'
#        print 'Named "{}"'.format(mesh.name)
        #print mesh.make_flattened_dict()

        for tri in mesh.tris:
            for (vi, ti, ni) in tri:
                print '{:25}{:25}'.format(mesh.vertices[vi-1],
                                          mesh.texcoords[ti-1])
