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
        'cflags': '-arch x86_64',
        'cxxflags': '-arch x86_64 -std=c++0x -stdlib=libc++',
    },

    'ios': {
        'cflags': '-arch armv7 -arch armv7s -arch arm64 '
                  '-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.1.sdk',
        'cxxflags': '-arch armv7 -arch armv7s -arch arm64 '
                    '-std=c++0x -stdlib=libc++ '
                    '-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS7.1.sdk',
        'configureflags': ['--host=arm-apple-darwin9'],
    },
}

MSGPACK_DEP = {
    'url': 'https://github.com/msgpack/msgpack-c/releases/download/cpp-0.5.8/msgpack-0.5.8.tar.gz',
    'sha1': 'e3100a327af5718cf4489d44c3b65f02e704aeb2',
    'unpack_name': 'msgpack-0.5.8',
    'name': 'msgpack',
    'install': True,
}

GTEST_DEP = {
    'url': 'https://googletest.googlecode.com/files/gtest-1.7.0.zip',
    'sha1': 'f85f6d2481e2c6c4a18539e391aa4ea8ab0394af',
    'unpack_name': 'gtest-1.7.0',
    'name': 'gtest',
    'install': False,
}

ALL_DEPS = [
    MSGPACK_DEP,
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


def build_dependencies_for_platform(platform_name):
    print 'Configuring and building dependencies for platform "%s"' % platform_name

    cflags = SUPPORTED_PLATFORMS[platform_name].get('cflags', '')
    cxxflags = SUPPORTED_PLATFORMS[platform_name].get('cxxflags', '')
    configureflags = SUPPORTED_PLATFORMS[platform_name].get('configureflags', [])

    env = {'CFLAGS': cflags, 'CXXFLAGS': cxxflags, 'CC': 'clang', 'CXX': 'clang++'}

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
        result = subprocess.call([abs_configure,
                                  '--prefix=%s' % dep_install_dir,
                                  '--disable-shared', '--enable-static',
                                  ] + configureflags,
                                 cwd=dep_build_dir, env=env)
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


def create_virtualenv():
    if os.path.exists('build-server/.venv'):
        return

    print 'Creating python virtualenv in build-server/.venv'
    result = subprocess.call(['virtualenv', 'build-server/.venv'])

    if result:
        raise Exception('could not create virtualenv')

    print 'Installing packages'
    result = subprocess.call(['./build-server/.venv/bin/pip', 'install',
                              '-r', 'build-server/requirements.txt'])

    if result:
        raise Exception('could not install python packages')


def run_gyp(platform_name):
    print 'Generating ninja build files in "out" folder'

    env = {'CC': 'clang', 'CXX': 'clang++'}

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
    build_dependencies_for_platform(args.platform)
    create_virtualenv()
    run_gyp(args.platform)

    return True


if __name__ == '__main__':
    if not main():
        sys.exit(1)
