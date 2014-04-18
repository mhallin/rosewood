'''Configure the Rosewood build setup'''

import argparse
import hashlib
import mmap
import os
import shutil
import subprocess
import sys

SUPPORTED_PLATFORMS = {
    'mac': {
        'developer_platform_name': 'MacOSX',
        'cflags': '-arch x86_64',
        'cxxflags': '-arch x86_64 -std=c++0x -stdlib=libc++',
    },

    'ios': {
        'developer_platform_name': 'iPhoneOS',
        'cflags': '-arch armv7 -arch armv7s -arch arm64',
        'cxxflags': '-arch armv7 -arch armv7s -arch arm64 -std=c++0x -stdlib=libc++',
        'configureflags': ['--host=arm-apple-darwin9'],
    },

    'emscripten': {
        'cc': os.path.abspath('./deps/emsdk_portable/emscripten/1.13.0/emcc'),
        'cxx': os.path.abspath('./deps/emsdk_portable/emscripten/1.13.0/em++'),
        'cxxflags': '-std=c++0x -stdlib=libc++',
        'skip_build_deps': True,
    },
}

GTEST_DEP = {
    'url': 'https://googletest.googlecode.com/files/gtest-1.7.0.zip',
    'sha1': 'f85f6d2481e2c6c4a18539e391aa4ea8ab0394af',
    'unpack_name': 'gtest-1.7.0',
    'name': 'gtest',
    'install': False,
}

ALL_DEPS = [
    GTEST_DEP
]


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--platform', '-p',
                        default='host',
                        action='store', dest='platform',
                        help='Platform to build for',
                        choices=SUPPORTED_PLATFORMS.keys() + ['host'])

    return parser.parse_args()


def determine_host_platform():
    if sys.platform == 'darwin':
        return 'mac'

    raise Exception('Unsupported host platform: "%s"' % sys.platform)


def ensure_deps_dirs_exist():
    for dirname in ['deps/src', 'deps/build']:
        if not os.path.exists(dirname):
            os.makedirs(dirname)


def fetch_file(url, dstname):
    code = subprocess.call(['curl', '-L', '-o', dstname, url])
    if code:
        raise Exception('curl completed unsuccessfully')


def verify_file_hash(filename, known_hash):
    print 'Verifying %s' % os.path.basename(filename)

    with open(filename, 'rb') as f:
        mm = mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ)

        file_hash = hashlib.sha1(mm).hexdigest()

        mm.close()

    if file_hash != known_hash:
        raise Exception('error while downloading %s' % os.path.basename(filename))


def download_dependency(dep):
    print 'Downloading %s' % os.path.basename(dep['url'])

    dstname = os.path.join('deps/src', os.path.basename(dep['url']))

    if not os.path.exists(dstname):
        fetch_file(dep['url'], dstname)

    verify_file_hash(dstname, dep['sha1'])

    return dstname


def extract_archive(filename):
    cwd = os.path.dirname(filename)
    basename = os.path.basename(filename)

    print 'Extracting %s' % os.path.basename(basename)

    if basename.endswith('.tar.gz'):
        result = subprocess.call(['tar', 'xzf', basename], cwd=cwd)
    elif basename.endswith('.zip'):
        result = subprocess.call(['unzip', '-q', basename], cwd=cwd)

    if result:
        raise Exception('could not extract %s' % basename)


def download_and_extract_dependencies():
    print 'Downloading and extracting missing dependencies'

    ensure_deps_dirs_exist()

    for dep in ALL_DEPS:
        unpack_path = os.path.join('deps/src', dep['unpack_name'])

        if not os.path.exists(unpack_path):
            dstname = download_dependency(dep)
            extract_archive(dstname)

    print 'All dependencies fetched and extracted'


def platform_isysroot(platform_name):
    name = SUPPORTED_PLATFORMS[platform_name].get('developer_platform_name', None)
    if name is None:
        return None

    platform_dir = '/Applications/Xcode.app/Contents/Developer/Platforms'
    sdk_dir = os.path.join(platform_dir, '%s.platform' % name, 'Developer/SDKs')

    sorted_sdks = sorted(os.listdir(sdk_dir), reverse=True)

    if not sorted_sdks:
        raise Exception('no sdks available for platform %s' % platform_name)

    return os.path.join(sdk_dir, sorted_sdks[0])


def copy_dependencies_for_platform(platform_name):
    print 'Copying dependency sources for platform "%s"' % platform_name

    platform = SUPPORTED_PLATFORMS[platform_name]

    for dep in ALL_DEPS:
        dep_dir = os.path.join('deps/src', dep['unpack_name'])
        dep_install_dir = os.path.abspath('deps/build/%s-%s' % (dep['name'], platform_name))

        if os.path.exists(dep_install_dir):
            print '%s already copied, skipping' % dep['name']
            continue

        print 'Copying %s source' % dep['name']
        shutil.copytree(dep_dir, dep_install_dir)


def build_dependencies_for_platform(platform_name):
    print 'Configuring and building dependencies for platform "%s"' % platform_name

    platform = SUPPORTED_PLATFORMS[platform_name]

    cc = platform.get('cc', 'clang')
    cxx = platform.get('cxx', 'clang++')
    cflags = platform.get('cflags', '')
    cxxflags = platform.get('cxxflags', '')
    platform_configure = platform.get('configure', None)
    configureflags = platform.get('configureflags', [])

    isysroot = platform_isysroot(platform_name)

    if isysroot:
        cflags += ' -isysroot %s' % isysroot
        cxxflags += ' -isysroot %s' % isysroot

    env = {'CFLAGS': cflags, 'CXXFLAGS': cxxflags, 'CC': cc, 'CXX': cxx}
    env.update(platform.get('extraenv', {}))

    for dep in ALL_DEPS:
        dep_dir = os.path.join('deps/src', dep['unpack_name'])
        dep_build_dir = 'deps/build/%s-%s-build' % (dep['name'], platform_name)
        dep_install_dir = os.path.abspath('deps/build/%s-%s' % (dep['name'], platform_name))

        abs_configure = os.path.abspath(os.path.join(dep_dir, 'configure'))

        if os.path.exists(dep_install_dir):
            print '%s already built and installed, skipping' % dep['name']
            continue

        if not os.path.exists(dep_build_dir):
            os.makedirs(dep_build_dir)

        print 'Configuring %s' % dep['name']
        args = ['--prefix=%s' % dep_install_dir,
                '--disable-shared', '--enable-static',
                ] + configureflags

        if platform_configure:
            args = [platform_configure, abs_configure] + args
        else:
            args = [abs_configure] + args
        result = subprocess.call(args, cwd=dep_build_dir, env=env)
        if result:
            raise Exception('error while configuring %s' % dep['name'])

        print 'Building %s' % dep['name']
        result = subprocess.call(['make'], cwd=dep_build_dir)

        if result:
            raise Exception('error while building %s' % dep['name'])

        if dep['install']:
            print 'Installing %s' % dep['name']
            result = subprocess.call(['make', 'install'], cwd=dep_build_dir)

            if result:
                raise Exception('error while insalling %s' % dep['name'])
        else:
            shutil.copytree(dep_dir, dep_install_dir)


def run_gyp(platform_name):
    print 'Generating ninja build files in "out" folder'

    platform = SUPPORTED_PLATFORMS[platform_name]

    cc = platform.get('cc', 'clang')
    cxx = platform.get('cxx', 'clang++')

    env = {'CC': cc, 'CXX': cxx}
    env.update(platform.get('extraenv', {}))

    result = subprocess.call(['./deps/gyp/gyp',
                              '--depth=.',
                              '-f', 'ninja',
                              '-DOS=%s' % platform_name,
                              'example/sample.gyp', 'rosewood.gyp'],
                              env=env)

    if result:
        raise Exception('gyp failed')


def main():
    args = parse_args()

    if args.platform == 'host':
        args.platform = determine_host_platform()

    download_and_extract_dependencies()

    if SUPPORTED_PLATFORMS[args.platform].get('skip_build_deps', False):
        copy_dependencies_for_platform(args.platform)
    else:
        build_dependencies_for_platform(args.platform)
    run_gyp(args.platform)

    return True


if __name__ == '__main__':
    if not main():
        sys.exit(1)
