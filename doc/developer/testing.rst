*******************
Testing Description
*******************

##################
Quick instructions
##################

To run the tests, simply execute the following command. Omit ``SR_PYTHON=On`` or ``SR_FORTRAN=On`` if you do not wish to test those languages:
   ``make test SR_PYTHON=On SR_FORTRAN=On``

####################
Running single tests
####################

With the increased automation in the SmartRedis test system, it is now more complex to run
an individual test case. The steps to follow are as follows:

1. Identify the test you wish to run.

This is most easily done by running the full test suite
and noting the name and path to the test. For example to run the ``test_config_set_command_DNE``
from within ``test_nonkeyed_cmd.py``, we'll save off the path:
``tests/python/test_nonkeyed_cmd.py::test_config_set_command_DNE``

.. code-block:: bash

  tests/python/test_nonkeyed_cmd.py::test_flushdb_command PASSED                                                 [ 87%]
  tests/python/test_nonkeyed_cmd.py::test_config_set_get_command PASSED                                          [ 88%]
  tests/python/test_nonkeyed_cmd.py::test_config_set_command_DNE PASSED                                          [ 88%]
  tests/python/test_nonkeyed_cmd.py::test_config_get_command_DNE PASSED                                          [ 89%]
  tests/python/test_nonkeyed_cmd.py::test_save_command PASSED                                                    [ 89%]

2. Launch a backend database.

Here, we'll set up a small Redis cluster with three nodes on the local
host at ports `6390`, `6391`, and `6392`, using the ``launch_redis.py`` script located in
the ``utils`` folder. If the ``nodes`` argument is not one, the launched nodes will be
clustered together; otherwise a standalone backend will be created.

.. code-block:: bash

   $ python utils/launch_redis.py --nodes 3 --port 6390 --rai v1.2.7

3. Set up environment variables.

These are established on your behalf when running the full test suite through the Makefile,
but here they must be set up manually. The variables to set are as follows:

 - SR_TEST_DEVICE={CPU, GPU}: defines the hardware environment in which tests are run
 - SR_DB_TYPE={Clustered, Standalone}: Defines the type of backend database established
 - SSDB: comma delimited list of IP addresses and ports for the nodes in the database

For this example, we would do:

.. code-block:: bash

   $ export SR_TEST_DEVICE=CPU
   $ export SR_DB_TYPE=Clustered
   $ export SSDB="127.0.0.1:6390;127.0.0.1:6391;127.0.0.1:6391"

4. Run the test by passing the path we stored previously to pytest:

.. code-block:: bash

  $ pytest -vv -s tests/python/test_nonkeyed_cmd.py::test_config_set_command_DNE

For the compiled tests (C, C++, Fortran), this is the easiest way to run them.
However, it is also possible to run the executables directly. Please note, however,
that many of these tests contain relative paths that point to shared resources, such
as AI models and scripts. To run these tests correctly, they need to be run from
their intended directory so that the relative paths line up correctly (this is done
on your behalf by the Python test wrappers for them). In particular, the tests
need to be run with the current working directory being the *SOURCE* directory for
the test. So for example, to run the test corresponding to ``client_test_delete_dataset.cpp``:

.. code-block:: bash

  $ # Option 1:
  $ pytest -vv -s tests/cpp/test_cpp_client.py::test_cpp_client[client_test_delete_dataset.cpp]
  $ # Option 2:
  $ cd tests/cpp
  $ ../../build/Release/tests/Shared/cpp/client_test_delete_dataset

5. Shut down the backend database when no longer needed:

.. code-block:: bash

  $ python utils/launch_redis.py --nodes 3 --port 6390 --stop

###################
Unit Test Framework
###################
All unit tests for the C++ client are located at ``tests/cpp/unit-tests/`` and use the Catch2
test framework. The unit tests mostly follow a Behavior Driven Development (BDD) style by
using Catch2's ``SCENARIO``, ``GIVEN``, ``WHEN``, and ``THEN`` syntax.

Files that contain Catch2 unit tests should be prefixed with *test_* in order to keep a
consistent naming convention.

When adding new unit tests, create a new ``SCENARIO`` in the appropriate file. If no such
file exists, then it is preferred that a new file (prefixed with *test_*) is created.

In Summary
===========

- New unit tests should be placed in ``tests/cpp/unit-tests/``
- Testing files should be prefixed with *test_*
- It is preferred that new unit tests are in a new ``SCENARIO``
