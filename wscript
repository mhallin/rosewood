# -*- mode: python -*-

import itertools
import operator
import os
import os.path

IBTOOL = 'ibtool ${IBFLAGS} ${SRC} --compile ${TGT}'
COPY_FILE = 'cp ${SRC} ${TGT}'

MODULES = ['ios', 'osx', 'ns',
           'libpng',
           'x11']

SRC_EXTS = ['cc', 'mm', 'c', 'm']

def _filter_modules(enabled_modules, nodes):
    """
    Filter out platform specific sources from a list of build
    nodes. Files belonging to `except_platform` will be kept.

    Platform-specific filenames end with _<platform> before the
    extension, e.g. image_loader_osx.mm, or if they reside in a path
    containing the platform name, e.g. src/ios/RWViewController.mm.
    """

    def allowed_node(node):
        name, _ = os.path.splitext(node.abspath())
        for mod in MODULES:
            mod_dir = '/' + mod + '/'
            mod_suffix = '_' + mod

            if mod in enabled_modules:
                continue
            if name.endswith(mod_suffix) or node.abspath().find(mod_dir) != -1:
                return False

        return True

    return filter(allowed_node, nodes)

def in_subdir(subdir, names):
    return [os.path.join(subdir, name) for name in names]

def options(opt):
    opt.load('compiler_c compiler_cxx')

    opt.add_option('--modules', action='store', default='', help='Rosewood modules to include')

def configure(conf):
    conf.env['CC'] = 'clang'
    conf.env['CXX'] = 'clang++'
    conf.env['RW_MODULES'] = conf.options.modules.split(',')

    conf.load('compiler_c compiler_cxx')

    warnings = ['-Wmissing-field-initializers', '-Wmissing-prototypes', '-Wreturn-type',
                '-Wnon-virtual-dtor', '-Woverloaded-virtual', '-Wno-exit-time-destructors',
                '-Wformat', '-Wmissing-braces', '-Wparentheses', '-Wswitch',
                '-Wunused-function', '-Wunused-label', '-Wunused-parameter',
                '-Wunused-variable', '-Wunused-value', '-Wempty-body', '-Wuninitialized',
                '-Wno-unknown-pragmas', '-Wno-shadow', '-Wno-four-char-constants',
                '-Wconversion', '-Wconstant-conversion', '-Wint-conversion',
                '-Wshorten-64-to-32', '-Wnewline-eof',
                '-Wno-c++11-extensions', '-Wno-sign-conversion']

    conf.env['CFLAGS'] = ['-std=c99', '-fobjc-arc', '-fcolor-diagnostics', '-msse3'] + warnings

    conf.env['CXXFLAGS'] = ['-std=c++11', '-stdlib=libc++', '-fobjc-arc', '-msse3',
                            '-fcolor-diagnostics'] + warnings

    conf.env['LINKFLAGS'] = ['-std=c++11', '-stdlib=libc++']
    conf.env['LIBPATH'] = ['/usr/lib', '/usr/local/lib']

    conf.env['INCLUDES'] = ['include', '/usr/local/include']

    conf.env['GTEST'] = 'gtest-1.6.0'

    if 'osx' in conf.env.RW_MODULES:
        conf.env.append_value('FRAMEWORK', 'Cocoa')
        conf.env.append_value('FRAMEWORK', 'OpenGL')
        conf.env.append_value('FRAMEWORK', 'CoreVideo')

    if 'x11' in conf.env.RW_MODULES:
        conf.env.append_value('LINKFLAGS', '-pthread')

        conf.check(header_name='GL/gl.h', features='c cprogram')
        conf.check(lib='GL', features='c cprogram')

        conf.check(header_name='thread', features='cxx cxxprogram')
        conf.check(lib='pthread', features='c cprogram')

    if 'libpng' in conf.env.RW_MODULES:
        conf.check(header_name='png.h', lib='png', features='c cprogram')

    conf.check(header_name='msgpack.h', features='c cprogram')
    conf.check(lib='msgpack', features='c cprogram')
    conf.check(header_name='ev.h', features='c cprogram')
    conf.check(lib='ev', features='c cprogram')

    env = conf.env

    conf.setenv('debug', env)
    conf.env.append_value('CFLAGS', ['-Os', '-g'])
    conf.env.append_value('CXXFLAGS', ['-Os', '-g'])

    conf.setenv('release', env)
    conf.env.append_value('CFLAGS', ['-Os'])
    conf.env.append_value('CXXFLAGS', ['-Os'])
    conf.env.append_value('LINKFLAGS', ['-flto'])

    conf.setenv('slow_debug', env)
    conf.env.append_value('CFLAGS', ['-O0', '-g', '-DCHECK_GL_CALLS'])
    conf.env.append_value('CXXFLAGS', ['-O0', '-g', '-DCHECK_GL_CALLS'])

def build(bld):
    if not bld.variant:
        bld.variant = 'debug'

    libs = ['msgpack', 'ev']

    if 'x11' in bld.env.RW_MODULES:
        libs.append('GL')
        libs.append('pthread')

    if 'libpng' in bld.env.RW_MODULES:
        libs.append('png')

    def sources_in_modules(root, modules):
        return reduce(operator.concat,
                      [bld.path.ant_glob('{}{}/*.{}'.format(root, '' if module == '.' else '/' + module, ext))
                       for module, ext in itertools.product(modules, SRC_EXTS)])

    modules = ['core', 'graphics', 'ios', 'math', 'osx', 'remote-control', 'utils', 'ns']
    example_modules = ['.', 'ios', 'osx', 'x11']

    rosewood_srcs = _filter_modules(bld.env.RW_MODULES, sources_in_modules('src', modules))
    example_srcs = _filter_modules(bld.env.RW_MODULES, sources_in_modules('example', example_modules))
    test_srcs = _filter_modules(bld.env.RW_MODULES, sources_in_modules('tests', ['.']))

    gtest_srcs = [bld.env.GTEST + '/src/gtest-all.cc']

    test_deps = in_subdir('src', [
            'core/component_array.cc', 'core/component.cc', 'core/transform.cc',
            'core/event.cc', 'core/entity.cc',
            'math/matrix3.cc', 'math/matrix4.cc',
            'math/quaternion.cc', 'math/vector.cc'])

    bld.program(features='c cxx',
                target='rwexample',
                source=(rosewood_srcs + example_srcs),
                lib=libs)

    bld.stlib(features='c cxx',
              target='gtest',
              defines=['GTEST_USE_OWN_TR1_TUPLE=1'],
              source=gtest_srcs,
              includes=[bld.env.GTEST + '/include', bld.env.GTEST])

    bld.program(features='c cxx',
                target='rosewood_tests',
                defines=['GTEST_USE_OWN_TR1_TUPLE=1'],
                source=(test_srcs + test_deps),
                includes=[bld.env.GTEST + '/include'],
                use='gtest')

from waflib import TaskGen
from waflib.Build import BuildContext, CleanContext
from waflib.Task import Task
from waflib.Tools.c import c_hook
from waflib.Tools.cxx import cxx_hook

from string import Template

def make_contexts(gen_variant):
    for ctx_cls in [BuildContext, CleanContext]:
        cmd_name = ctx_cls.__name__.replace('Context', '').lower()
        class ctx_subcls(ctx_cls):
            cmd = cmd_name + '_' + gen_variant
            variant = gen_variant

for variant in ['debug', 'release', 'slow_debug']:
    make_contexts(variant)

@TaskGen.extension('.mm')
def mm_hook(self, node):
    return cxx_hook(self, node)

@TaskGen.extension('.m')
def m_hook(self, node):
    return c_hook(self, node)
