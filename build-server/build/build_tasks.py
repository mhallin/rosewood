import os
import shutil
import inspect
import re
import json
import rbdef
import toml

from os import path

import fbx_importer as fbx

from build_graph import TaskNode, connect_nodes, disconnect_nodes

BUILD_TASKS = {}


def build_task_single(regex, name=None, output_map_fn=None):

    def inner(fn_or_cls):
        if inspect.isfunction(fn_or_cls):
            class FunctionTaskNode(TaskNode):
                def __init__(self, bld, srcs, dests):
                    i = list(srcs)[0]
                    o = bld.as_output_node(output_map_fn(i.filename))

                    super(FunctionTaskNode, self).__init__(bld, srcs, [o])

                @property
                def name(self):
                    return name

                def run(self, bld):
                    i = list(self.edges_in)[0]
                    o = list(self.edges_out)[0]
                    return fn_or_cls(i, o)

            BUILD_TASKS[re.compile(regex)] = FunctionTaskNode

        else:
            def wrapper(bld, srcs, dests):
                i = list(srcs)[0]
                o = list(dests)[0] if dests else None
                return fn_or_cls(bld, i, o)

            BUILD_TASKS[re.compile(regex)] = wrapper

        return fn_or_cls

    return inner


def ensure_dir_for_file_exists(filename):
    dirname = path.dirname(filename)
    if not path.exists(dirname):
        os.makedirs(dirname)


def change_ext(p, new_ext):
    base, _ = path.splitext(p)
    return base + new_ext


def should_ignore(filename):
    bname = path.basename(filename)
    return bname[0] == '.' or bname[-1] == '~'


def build_file(bld, path):
    if should_ignore(path):
        return

    for regex, fn in BUILD_TASKS.iteritems():
        if regex.search(path):
            i = bld.as_file_node(path)
            task = fn(bld, [i], [])
            bld.add_task(task)
            break


@build_task_single(r'\.mesh$')
class BuildJSONMeshTask(TaskNode):
    def __init__(self, bld, src, dest):
        if not dest:
            dest = bld.as_output_node(change_ext(src.filename, '.mesh-rbdef'))

        self.mesh_node = src
        self.mesh_dest_node = None

        super(BuildJSONMeshTask, self).__init__(bld, [src], [])

        self._update_dependents(bld)

    @property
    def name(self):
        return 'Compile JSON Mesh'

    @property
    def is_valid(self):
        return self.mesh_node.is_valid

    def run(self, bld):
        self._update_dependents(bld)

        with file(self.mesh_node.filename) as infile:
            data = json.load(infile)

        vertices = data['vertices']
        normals = {'': data['normals']}
        texcoords = {'': data['texcoords']}

        ensure_dir_for_file_exists(self.mesh_dest_node.filename)

        with file(self.mesh_dest_node.filename, 'wb') as outfile:
            rbdef.dump([vertices, normals, texcoords], outfile)

        return True

    def _update_dependents(self, bld):
        if self.mesh_dest_node:
            disconnect_nodes(self, self.mesh_dest_node)
            self.mesh_dest_node = None

        src_filename = self.mesh_node.filename

        self.mesh_dest_node = bld.as_output_node(change_ext(src_filename, '.mesh-rbdef'))
        connect_nodes(self, self.mesh_dest_node)


@build_task_single(r'\.fbx$')
class BuildFBXMeshTask(TaskNode):
    default_import_settings = {'normals': 'from-mesh',
                               'normals-recalculate-name': None,
                               'normals-recalculate-angle': 60,
                               'generate-mesh': True,
                               'generate-convex-hull': False,
                               'import-name': None}
    def __init__(self, bld, src, dest):
        if not dest:
            dest = bld.as_output_node(change_ext(src.filename, '.mesh-rbdef'))

        self.settings_node = bld.as_file_node(src.filename + '.rwsettings')
        self.mesh_node = src

        self.mesh_dest_node = None
        self.hull_dest_node = None

        super(BuildFBXMeshTask, self).__init__(bld,
                                               [src, self.settings_node],
                                               [])


        self._update_dependents(bld)

    @property
    def name(self):
        return 'Compile FBX'

    @property
    def is_valid(self):
        return self.mesh_node.is_valid

    def run(self, bld):
        self._update_dependents(bld)

        with file(self.mesh_node.filename) as fbxfile:
            objs = fbx.parse_fbx(fbxfile)['Objects']

        import_settings = self._load_import_settings()
        mesh = self._load_mesh(objs, import_settings['import-name'])

        if import_settings['normals'] == 'recalculate':
            mesh.recalculate_normals(import_settings['normals-recalculate-name'])
            mesh.smooth_normals(import_settings['normals-recalculate-angle'],
                                import_settings['normals-recalculate-name'])

        if import_settings['generate-mesh']:
            self._write_mesh(mesh)

        if import_settings['generate-convex-hull']:
            self._write_convex_hull(mesh)

        return True

    def _load_import_settings(self):
        import_settings = dict(self.default_import_settings)
        if os.path.exists(self.settings_node.filename):
            with file(self.settings_node.filename) as settingsfile:
                import_settings.update(toml.load(settingsfile))

        return import_settings

    def _load_mesh(self, objects, mesh_name):
        for model in objects.find_objects('Model'):
            if (mesh_name is None or model.properties[0] == mesh_name) and model.properties[1] == 'Mesh':
                return fbx.construct_mesh(model)

        return None

    def _write_mesh(self, mesh):
        data = mesh.make_flattened_dict()

        vertices = data['vertices']
        normals = data['normals']
        texcoords = data['uvs']

        ensure_dir_for_file_exists(self.mesh_dest_node.filename)

        with file(self.mesh_dest_node.filename, 'w') as mesh_output_file:
            rbdef.dump([vertices, normals, texcoords], mesh_output_file)

    def _write_convex_hull(self, mesh):
        vertices = set()
        for poly in mesh.polygons:
            vertices.update(poly)

        ensure_dir_for_file_exists(self.hull_dest_node.filename)

        with file(self.hull_dest_node.filename, 'w') as hull_output_file:
            rbdef.dump(fbx.flatten(vertices), hull_output_file)

    def _update_dependents(self, bld):
        if self.mesh_dest_node:
            disconnect_nodes(self, self.mesh_dest_node)
            self.mesh_dest_node = None

        if self.hull_dest_node:
            disconnect_nodes(self, self.hull_dest_node)
            self.hull_dest_node = None

        import_settings = self._load_import_settings()

        src_filename = self.mesh_node.filename

        if import_settings['generate-mesh']:
            self.mesh_dest_node = bld.as_output_node(change_ext(src_filename, '.mesh-rbdef'))
            connect_nodes(self, self.mesh_dest_node)

        if import_settings['generate-convex-hull']:
            self.hull_dest_node = bld.as_output_node(change_ext(src_filename, '.hull-rbdef'))
            connect_nodes(self, self.hull_dest_node)


@build_task_single('\.png$', 'Copy PNG', lambda p: p)
def copy_png(i, o):
    ensure_dir_for_file_exists(o.filename)

    shutil.copy(i.filename, o.filename)

    return True


@build_task_single('\.json$', 'JSON to RBDEF', lambda p: change_ext(p, '.rbdef'))
def json_to_rbdef(i, o):
    ensure_dir_for_file_exists(o.filename)

    with file(i.filename) as infile:
        data = json.load(infile)

    with file(o.filename, 'w') as outfile:
        rbdef.dump(data, outfile)

    return True


@build_task_single('\.rwshader$')
class BuildShaderTask(TaskNode):
    def __init__(self, bld, src, dest):
        if not dest:
            dest = bld.as_output_node(change_ext(src.filename, '.rwshader-rbdef'))

        super(BuildShaderTask, self).__init__(bld, [src], [dest])

        self.shader_metadata = src
        self.vertex_shader = None
        self.fragment_shader = None
        self.destination = dest

        self._update_dependencies(bld)

    @property
    def name(self):
        return 'Compile Shader'

    @property
    def is_valid(self):
        return self.shader_metadata.is_valid

    def run(self, bld):
        self._update_dependencies(bld)

        with file(self.shader_metadata.filename) as mdfile:
            shader_input = toml.load(mdfile)

        try:
            with file(self.vertex_shader.filename) as vshfile:
                shader_input['vertex-shader'] = vshfile.read()

            with file(self.fragment_shader.filename) as fshfile:
                shader_input['fragment-shader'] = fshfile.read()
        except IOError:
            return False

        ensure_dir_for_file_exists(self.destination.filename)

        with file(self.destination.filename, 'w') as ofile:
            rbdef.dump(shader_input, ofile)

        return True

    def _update_dependencies(self, bld):
        if self.vertex_shader and self.fragment_shader:
            disconnect_nodes(self.vertex_shader, self)
            disconnect_nodes(self.fragment_shader, self)

        with file(self.shader_metadata.filename) as shader_file:
            shader_metadata = toml.load(shader_file)

        srcdir = path.dirname(self.shader_metadata.filename)

        vsh = bld.as_file_node(path.join(srcdir,
                                         shader_metadata['vertex-shader']))
        fsh = bld.as_file_node(path.join(srcdir,
                                         shader_metadata['fragment-shader']))

        self.vertex_shader = vsh
        self.fragment_shader = fsh

        connect_nodes(self.vertex_shader, self)
        connect_nodes(self.fragment_shader, self)
