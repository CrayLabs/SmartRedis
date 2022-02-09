# BSD 2-Clause License
#
# Copyright (c) 2021-2022, Hewlett Packard Enterprise
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os
import sys
import subprocess
import shutil
from pathlib import Path
import multiprocessing as mp

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

# get number of processors
NPROC = mp.cpu_count()

class CMakeExtension(Extension):
    def __init__(self, name):
        Extension.__init__(self, name, sources=[])

class CMakeBuild(build_ext):

    @property
    def cmake(self):
        """Find and use installed cmake"""
        cmake_cmd = shutil.which("cmake")
        return cmake_cmd

    def run(self):
        check_prereq("make")
        check_prereq("gcc")
        check_prereq("g++")

        build_directory = Path(self.build_temp).resolve()
        cmake_args = [
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + str(build_directory),
            '-DPYTHON_EXECUTABLE=' + sys.executable
        ]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]
        build_args += ['--', f'-j{str(NPROC)}']
        self.build_args = build_args

        cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]

        # setup build environment
        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())

        # make tmp dir
        if not build_directory.is_dir():
            os.makedirs(self.build_temp)

        print('-'*10, 'Building C dependencies', '-'*40)
        make_cmd = shutil.which("make")
        setup_path = Path(os.path.abspath(os.path.dirname(__file__))).resolve()

        # build dependencies
        subprocess.check_call([f"{make_cmd} deps"],
                              cwd=setup_path,
                              shell=True)

        # run cmake prep step
        print('-'*10, 'Running CMake prepare', '-'*40)
        subprocess.check_call([self.cmake, setup_path] + cmake_args,
                              cwd=build_directory,
                              env=env)


        print('-'*10, 'Building extensions', '-'*40)
        cmake_cmd = [self.cmake, '--build', '.'] + self.build_args
        subprocess.check_call(cmake_cmd,
                              cwd=build_directory)

        shutil.copytree(setup_path.joinpath("install"),
                        build_directory.joinpath("install"))

        # Move from build temp to final position
        for ext in self.extensions:
            self.move_output(ext)

    def move_output(self, ext):
        build_temp = Path(self.build_temp).resolve()
        dest = Path(self.get_ext_fullpath(ext.name)).parent.absolute()
        dest_path = dest.joinpath("smartredis")
        source_path = build_temp / self.get_ext_filename(ext.name)
        dest_directory = dest_path.parents[0]
        dest_directory.mkdir(parents=True, exist_ok=True)
        self.copy_file(source_path, dest_path)

# check that certain dependencies are installed
# TODO: Check versions for compatible versions
def check_prereq(command):
    try:
        out = subprocess.check_output([command, '--version'])
    except OSError:
        raise RuntimeError(
            f"{command} must be installed to build SmartRedis")

# update existing env var
def update_env_var(var, new):
    try:
        value = os.environ[var]
        value = ":".join((value, str(new)))
        return value
    except KeyError:
        return new

ext_modules = [
    CMakeExtension('smartredisPy'),
]

setup(
 # ... in setup.cfg
    packages=find_packages(),
    ext_modules=ext_modules,
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
