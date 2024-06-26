# BSD 2-Clause License
#
# Copyright (c) 2021-2024, Hewlett Packard Enterprise
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

import pytest
from os import path as osp
from glob import glob
import pathlib
import time

RANKS = 1
TEST_PATH = pathlib.Path(__file__).resolve().parent

def get_test_names():
    """Obtain test names by globbing for client_test
    Add tests manually if necessary
    """
    glob_path_1 = TEST_PATH.glob("*/*/example*")
    glob_path_2 = TEST_PATH.glob("*/*/smartredis*")
    test_names = list(glob_path_1) + list(glob_path_2)
    test_names = list(filter(lambda test: test.find('example_utils') == -1, test_names))
    test_names = list(filter(lambda test: test.find('.py') == -1, test_names))
    test_names = [(pytest.param(test,
                                id=osp.basename(test))) for test in test_names]
    return test_names

@pytest.mark.parametrize("test", get_test_names())
def test_example(test, bin_path, build_fortran, execute_cmd):
    if (build_fortran == "ON" or ".F90" not in test):
        # Build the path to the test executable from the source file name
        # . keep only the last three parts of the path: (parallel/serial, language, basename)
        test = "/".join(test.split("/")[-3:])
        test_subdir = "/".join(test.split("/")[0:2])
        # . drop the file extension
        test = ".".join(test.split(".")[:-1])
        # . prepend the path to the built test executable
        test = bin_path / test
        cmd = ["mpirun", "-n", "2"] if "parallel" in test else []
        cmd += [test]
        print(f"\nRunning test: {test.basename()}")
        print(f"Test command {' '.join(cmd)}")
        execute_cmd(cmd, pathlib.Path.cwd()/test_subdir)
        time.sleep(1)
    else:
        print (f"Skipping Fortran test {test}")
