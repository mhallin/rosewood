import os
import sys
import hashlib

import rbdef

from os import path


def connect_nodes(from_node, to_node):
    from_node.edges_out.add(to_node)
    to_node.edges_in.add(from_node)


def disconnect_nodes(from_node, to_node):
    from_node.edges_out.remove(to_node)
    to_node.edges_in.remove(from_node)


def hash_file(abspath):
    s = hashlib.sha1()
    with file(abspath, 'r') as f:
        s.update(f.read())

    return s.digest()


class BuildNode(object):
    def __init__(self):
        super(BuildNode, self).__init__()
        self.edges_in = set()
        self.edges_out = set()

    @property
    def is_file(self):
        return False

    @property
    def is_task(self):
        return False

    @property
    def is_valid(self):
        return True


class FileNode(BuildNode):
    def __init__(self, filename):
        super(FileNode, self).__init__()

        self.filename = filename
        self.build_cache = {}

    @property
    def is_file(self):
        return True

    @property
    def is_valid(self):
        return path.exists(self.filename)

    def cache_build(self, source_nodes):
        self.build_cache = {}
        for n in (n for n in source_nodes if path.exists(n.filename)):
            self.build_cache[n.filename] = hash_file(n.filename)

    def load_build_cache(self, source_nodes, cache):
        self.build_cache = {}
        for n in filter(lambda n: n.filename in cache, source_nodes):
            self.build_cache[n.filename] = cache[n.filename]

    def is_invalid(self, source_nodes):
        if not path.exists(self.filename):
            return True

        def node_is_invalid(n):
            fn = n.filename

            if fn in self.build_cache:
                if path.exists(fn):
                    return self.build_cache[fn] != hash_file(fn)
                else:
                    return True
            else:
                return path.exists(fn)

        return any(map(node_is_invalid, source_nodes))


class TaskNode(BuildNode):
    def __init__(self, bld, inputs, outputs):
        super(TaskNode, self).__init__()

        for i in inputs:
            connect_nodes(i, self)

        for o in outputs:
            connect_nodes(self, o)

    @property
    def is_task(self):
        return True

    @property
    def is_valid(self):
        return all(n.is_valid for n in self.edges_in)

    @property
    def is_output_invalid(self):
        for o in self.edges_out:
            if o.is_invalid(self.edges_in):
                return True

        return False

    @property
    def name(self):
        return '(TaskNode)'

    def run(self, bld):
        subclass_responsibility

    def __repr__(self):
        return '<\'{}\' at 0x{}>'.format(self.name, id(self))


class BuildGraph(object):
    def __init__(self, root, out_dir='asset-build'):
        super(BuildGraph, self).__init__()

        self.file_map = {}
        self.tasks = []
        self.root = root
        self.out_dir = out_dir

    def file(self, filename):
        abspath = path.join(self.root, filename)

        if abspath not in self.file_map:
            self.file_map[abspath] = FileNode(abspath)

        return self.file_map[abspath]

    def add_task(self, task):
        self.tasks.append(task)
        return task

    def as_file_node(self, f):
        if isinstance(f, FileNode):
            return f
        return self.file(f)

    def as_output_node(self, src_node):
        if hasattr(src_node, 'filename'):
            src = src.filename
        else:
            src = src_node

        comps = path.relpath(src, self.root).split('/')
        return self.file(path.join(self.out_dir, path.join(*comps[1:])))

    def run_task(self, task):
        in_strs = [n.filename for n in task.edges_in]
        out_strs = [n.filename for n in task.edges_out]
        print 'running task {} => {} => {}'.format(in_strs,
                                                   task.name,
                                                   out_strs)
        if not task.run(self):
            print ' ** FAILED: {}'.format(task.name)
            return False

        for o in task.edges_out:
            o.cache_build(task.edges_in)

        return True

    def remove_node(self, node):
        for i in frozenset(node.edges_in):
            disconnect_nodes(i, node)

        for o in frozenset(node.edges_out):
            disconnect_nodes(node, o)

        if node.is_task:
            self.tasks.remove(node)
        else:
            del self.file_map[node.filename]

    @property
    def build_cache_filename(self):
        return path.join(self.root, self.out_dir, 'rw_build_cache.rbdef')

    def save_build_cache(self):
        bc = {}
        for n in self.file_map.itervalues():
            for filepath, hash in n.build_cache.iteritems():
                bc[filepath] = hash

        with file(self.build_cache_filename, 'wb') as bcfile:
            rbdef.dump(bc, bcfile)

    def load_build_cache(self):
        if not path.exists(self.build_cache_filename):
            return

        with file(self.build_cache_filename, 'rb') as bcfile:
            bc = rbdef.load(bcfile)

        for t in self.tasks:
            for o in t.edges_out:
                o.load_build_cache(t.edges_in, bc)


def recursive_build(bld, root):
    from build_tasks import build_file

    for dirpath, dirnames, filenames in os.walk(root):
        for filename in filenames:
            build_file(bld, path.join(dirpath, filename))


def generate_graphviz(bld, f):
    print >>f, 'digraph bld {'
    print >>f, '  rankdir=LR'
    for task in bld.tasks:
        tn = repr(task)

        for i in task.edges_in:
            fn = path.relpath(i.filename, bld.root)
            print >>f, '  "{}" -> "{}"'.format(fn, tn)

        for o in task.edges_out:
            fn = path.relpath(o.filename, bld.root)
            print >>f, '  "{}" -> "{}"'.format(tn, fn)

        print >>f, '  "{}" [label="{}", shape=box]'.format(tn, task.name)

    print >>f, '}'


def default_graph():
    bld = BuildGraph(os.getcwd())
    recursive_build(bld, os.path.join(os.getcwd(), 'game-assets'))
    return bld


def main():
    bld = default_graph()

    if len(sys.argv) < 2:
        print 'usage: build_graph.py command'
        return False

    if sys.argv[1] == 'dot':
        generate_graphviz(bld, sys.stdout)

    elif sys.argv[1] == 'build':
        bld.load_build_cache()

        for task in bld.tasks:
            if task.is_output_invalid:
                bld.run_task(task)

        bld.save_build_cache()

    return True


if __name__ == '__main__':
    if main():
        sys.exit(1)
