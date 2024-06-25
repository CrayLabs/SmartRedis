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

import pathlib
import pytest
from os import environ
import time

test_gpu = environ.get("SR_TEST_DEVICE","cpu").lower() == "gpu"

RANKS = 1
TEST_PATH = pathlib.Path(__file__).resolve().parent

def get_test_names():
    """Obtain test names by globbing for client_test
    Add tests manually if necessary
    """
    glob_path = TEST_PATH
    test_names = glob_path.glob("client_test*.F90")
    test_names = list(filter(lambda test: str(test).find('gpu') == -1, test_names))
    test_names = [(pytest.param(test,
                                id=test.name)) for test in test_names]
    return test_names

@pytest.mark.parametrize("test", get_test_names())
def test_fortran_client(test, bin_path, execute_cmd):
    """This function actually runs the tests using the parameterization
    function provided in Pytest

    :param test: a path to a test to run
    :type test: str
    """
    # Build the path to the test executable from the source file name
    # . keep only the last three parts of the path: (language, basename)
    basename = test.stem
    language = test.parent.name
    test = bin_path / language / "bin" / basename
    cmd = [str(test)]
    execute_cmd(cmd, TEST_PATH)
    time.sleep(1)

@pytest.mark.skipif(
    not test_gpu,
    reason="SR_TEST_DEVICE does not specify 'gpu'"
)
def test_client_multigpu_mnist():
    """
    Test setting and running a machine learning model via the Fortran client
    on an orchestrator with multiple GPUs
    """
    tester_path = TEST_PATH / "client_test_mnist_multigpu.F90"
    test_fortran_client(tester_path)

