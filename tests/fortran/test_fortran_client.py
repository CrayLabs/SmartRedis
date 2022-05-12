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

import pytest
import os
from glob import glob
from shutil import which
from subprocess import Popen, PIPE, TimeoutExpired
import time

test_gpu = os.environ.get("SMARTREDIS_TEST_DEVICE","cpu").lower() == "gpu"

RANKS = 1
TEST_PATH = os.path.dirname(os.path.abspath(__file__))

def get_test_names():
    """Obtain test names by globbing for client_test
    Add tests manually if necessary
    """
    glob_path = os.path.join(TEST_PATH, "build/client_test*")
    test_names = glob(glob_path)
    test_names = [(pytest.param(test, id=os.path.basename(test)))
                  for test in test_names if not "gpu" in test]
    return test_names


@pytest.mark.parametrize("test", get_test_names())
def test_fortran_client(test):
    """This function actually runs the tests using the parameterization
    function provided in Pytest

    :param test: a path to a test to run
    :type test: str
    """
    cmd = []
    cmd.append(test)
    print(f"Running test: {os.path.basename(test)}")
    print(f"Test command {' '.join(cmd)}")
    execute_cmd(cmd)
    time.sleep(1)

def execute_cmd(cmd_list):
    """Execute a command """

    # spawning the subprocess and connecting to its output
    run_path = os.path.join(TEST_PATH, "build/")
    proc = Popen(
        cmd_list, stderr=PIPE, stdout=PIPE, stdin=PIPE, cwd=run_path)
    try:
        out, err = proc.communicate(timeout=120)
        if out:
            print("OUTPUT:", out.decode("utf-8"))
        if err:
            print("ERROR:", err.decode("utf-8"))
        assert(proc.returncode == 0)
    except UnicodeDecodeError:
        output, errs = proc.communicate()
        print("ERROR:", errs.decode("utf-8"))
        assert(False)
    except TimeoutExpired:
        proc.kill()
        output, errs = proc.communicate()
        print("TIMEOUT: test timed out after test timeout limit of 120 seconds")
        print("OUTPUT:", output.decode("utf-8"))
        print("ERROR:", errs.decode("utf-8"))
        assert(False)
    except Exception:
        proc.kill()
        output, errs = proc.communicate()
        print("OUTPUT:", output.decode("utf-8"))
        print("ERROR:", errs.decode("utf-8"))
        assert(False)

@pytest.mark.skipif(
    not test_gpu,
    reason="SMARTREDIS_TEST_DEVICE does not specify 'gpu'"
)
def test_client_multigpu_mnist():
    """
    Test setting and running a machine learning model via the Fortran client
    on an orchestrator with multiple GPUs
    """
    
    tester_path = os.path.join(TEST_PATH, "build/client_test_mnist_multigpu")
    test_fortran_client(tester_path)
    
