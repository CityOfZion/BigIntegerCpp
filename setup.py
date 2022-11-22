import sys
import os
import re
import platform
import subprocess

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

if sys.version_info < (3, 8):
    sys.exit('Python < 3.8 is not supported')

with open('bindings/python/python-bindings.cpp','r') as f:
    text = f.read()
    regex = re.compile(r'.*VERSION_BINDINGS = "(.*)"')
    match = re.search(regex, text)
    version = match.group(1)


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.15.0':
                raise RuntimeError("CMake >= 3.15.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        # required for auto-detection of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DBIGINTEGER_BUILD_PYTHON_BINDINGS=ON',
                      '-DBIGINTEGER_BUILD_SHARED_LIB=OFF',
                      '-DBIGINTEGER_BUILD_STATIC_LIB=OFF',
                      '-DBUILD_TESTING=OFF']

        # cfg = 'Debug' if self.debug else 'Release'
        cfg = 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']
            if sys.platform.startswith("darwin"):
                # Cross-compile support for macOS - respect ARCHFLAGS if set
                archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
                if archs:
                    cmake_args += ["-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)


with open('README.md') as readme_file:
    readme = readme_file.read()

setup(
    author='Erik van den Brink (ixje), merl111',
    author_email='erik@coz.io',
    name='pybiginteger',
    description="C++ port of the C# BigInteger class",
    long_description=readme,
    long_description_content_type="text/markdown",
    version=version,
    url='https://github.com/CityOfZion/BigIntegerCpp',
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        "Programming Language :: C++"
    ],
    ext_modules=[CMakeExtension('pybiginteger', sourcedir='')],
    cmdclass=dict(build_ext=CMakeBuild)
)
