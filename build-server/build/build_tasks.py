import os
import shutil
import inspect
import re
import json
import msgpack

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


@build_task_single(r'\.fbx$')
class BuildMeshTask(TaskNode):
    default_import_settings = {'normals': 'from-mesh',
                               'normals-recalculate-name': None,
                               'normals-recalculate-angle': 60}
    def __init__(self, bld, src, dest):
        if not dest:
            dest = bld.as_output_node(change_ext(src.filename, '.mesh-mp'))

        self.settings_node = bld.as_file_node(src.filename + '.rwsettings')

        super(BuildMeshTask, self).__init__(bld,
                                            [src, self.settings_node],
                                            [dest])

        self.mesh_node = src
        self.dest_node = dest

    @property
    def name(self):
        return 'Compile FBX'

    @property
    def is_valid(self):
        return self.mesh_node.is_valid

    def run(self, bld):
        with file(self.mesh_node.filename) as fbxfile:
            objs = fbx.parse_fbx(fbxfile)['Objects']

        import_settings = dict(self.default_import_settings)
        if os.path.exists(self.settings_node.filename):
            with file(self.settings_node.filename) as settingsfile:
                import_settings.update(json.load(settingsfile))

        models = objs.find_objects('Model')
        mesh_object = [obj for obj in models if obj.properties[1] == 'Mesh'][0]
        mesh = fbx.construct_mesh(mesh_object)

        if import_settings['normals'] == 'recalculate':
            mesh.recalculate_normals(import_settings['normals-recalculate-name'])

        data = mesh.make_flattened_dict()

        vertices = data['vertices']
        normals = data['normals']
        texcoords = data['uvs']

        ensure_dir_for_file_exists(self.dest_node.filename)

        with file(self.dest_node.filename, 'w') as mesh_output_file:
            msgpack.pack([vertices, normals, texcoords], mesh_output_file)

        return True


@build_task_single('\.png$', 'Copy PNG', lambda p: p)
def copy_png(i, o):
    ensure_dir_for_file_exists(o.filename)

    shutil.copy(i.filename, o.filename)

    return True


@build_task_single('\.rwshader$')
class BuildShaderTask(TaskNode):
    def __init__(self, bld, src, dest):
        if not dest:
            dest = bld.as_output_node(change_ext(src.filename, '.rwshader-mp'))

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
            shader_input = json.load(mdfile)

        try:
            with file(self.vertex_shader.filename) as vshfile:
                shader_input['vertex-shader'] = vshfile.read()

            with file(self.fragment_shader.filename) as fshfile:
                shader_input['fragment-shader'] = fshfile.read()
        except IOError:
            return False

        ensure_dir_for_file_exists(self.destination.filename)

        with file(self.destination.filename, 'w') as ofile:
            msgpack.pack(shader_input, ofile)

        return True

    def _update_dependencies(self, bld):
        if self.vertex_shader and self.fragment_shader:
            disconnect_nodes(self.vertex_shader, self)
            disconnect_nodes(self.fragment_shader, self)

        with file(self.shader_metadata.filename) as shader_file:
            shader_metadata = json.load(shader_file)

        srcdir = path.dirname(self.shader_metadata.filename)

        vsh = bld.as_file_node(path.join(srcdir,
                                         shader_metadata['vertex-shader']))
        fsh = bld.as_file_node(path.join(srcdir,
                                         shader_metadata['fragment-shader']))

        self.vertex_shader = vsh
        self.fragment_shader = fsh

        connect_nodes(self.vertex_shader, self)
        connect_nodes(self.fragment_shader, self)
