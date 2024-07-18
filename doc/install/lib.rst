For C, C++, and Fortran applications, the SmartRedis client library will need
to be compiled and linked in. The build system is CMake-based but we also
offer a Makefile that cover the requirements for most users.

Downloading SmartRedis via the Github release page is possible, however we recommend
that most users clone the repository via git

.. code-block:: bash

    git clone https://github.com/CrayLabs/SmartRedis.git [--branch tag_name] smartredis


CMake-based build
-----------------

The ``CMakeLists.txt`` file in the SmartRedis root directory is fully self contained
and can be used directly to compile and install the SmartRedis library. At a bare
minimum users, can compile the library with

.. code-block:: bash

    cd /path/to/SmartRedis
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install/
    make install -j

which will compile the C and C++ interfaces to SmartRedis and install the libraries
and all header files to ``/path/to/install/location``.

For other CMake-based projects, the config files for SmartRedis will be in the
`share/cmake/smartredis` subdirectory and can be included by modifying the
application's ``CMakeLists.txt``

.. code-block:: cmake

    find_package(smartredis REQUIRED)
    target_link_libraries(example_target smartredis)
    target_include_directories(example_target ${smartredis_INCLUDE_DIR})

For more advanced used we provide the following options, their default settings,
and the allowed values which can be set at configuration time via
``-DEXAMPLE_PARAMETER=EXAMPLE_VALUE``

.. list-table:: SmartRedis CMake options
   :widths: 25 10 25 50
   :header-rows: 1

   * - Parameter name
     - Default
     - Allowed values
     - Description
   * - BUILD_FORTRAN
     - OFF
     - ON, OFF
     - Build the Fortran client library in addition to C/C++
   * - BUILD_PYTHON
     - OFF
     - ON, OFF
     - Build the Python client in addition to C/C++ (useful if the SmartRedis
       python package was installed in editable mode)
   * - BUILD_SHARED_LIBS
     - ON
     - ON, OFF
     - If ``ON``, build SmartRedis libraries as a dynamic library, otherwise
       as a static library
   * - CMAKE_BUILD_TYPE
     - Release
     - Release, Debug, Coverage
     - Determine whether the optimized, debuggable, or debuggable with coverage
       version of SmartRedis will be compiled
   * - CMAKE_INSTALL_PREFIX
     - ``/usr/local``
     - Any path
     - Install path prefix, prepended onto install directories.
   * - PEDANTIC
     - OFF
     - ON, OFF
     - If ON, error out if any compiler warnings are found
   * - RETAIN_RPATH
     - ON
     - ON, OFF
     - If ON, retain the path to the libsmartredis library for the Fortran library
       when using dynamic linking


Makefile-based build
--------------------

The ```Makefile`` included in the top level of the SmartRedis repository has two
main targets: ``lib`` which will create a dynamic library for C, C++, and
(optionally) Fortran and Python clients; and ``lib-with-fortran`` which will also
build a library for Fortran applications. ``make help`` will list additional
targets that are used for SmartRedis development.

.. code-block:: bash

  cd SmartRedis
  make lib #or lib-with-fortran

By default, the SmartRedis library will be compiled as a shared library with the
system default compilers and installed in ``SmartRedis/install/lib/`` and the
SmartRedis header files (and optionally the Fortran ``.mod`` files) will be
installed in ``SmartRedis/install/include/``.

.. code-block:: bash

    # GCC Toolchain
    make lib-with-fortran CC=foo CXX=bar FC=baz

For reference, the appropriate values for ``CC``, ``CXX``, and ``FC`` for the compiler
chains we support are:

.. list-table:: Values for compiler chains
    :widths: 20 12 12 12
    :header-rows: 1

    * - Compiler Chain
      - ``CC``
      - ``CXX``
      - ``FC``
    * - GCC
      - ``gcc``
      - ``g++``
      - ``gfortran``
    * - Intel (Classic)
      - ``icc``
      - ``icpc``
      - ``ifort``
    * - Intel (OneAPI)
      - ``icx``
      - ``icpx``
      - ``ifx``
    * - NVHPC
      - ``nvc``
      - ``nvc++``
      - ``nvfortran``

By default, the SmartRedis library is built as a shared library. For some
applications, however, it is preferable to link to a statically compiled
library. This can be done  by specifying the ``LINK_TYPE``

.. code-block:: bash

    make lib LINK_TYPE=shared # Default shared
    make lib LINK_TYPE=static # Static build

Linked statically, the SmartRedis library will have a ``.a`` file extension.  When
linked dynamically, the SmartRedis library will have a ``.so`` file extension.

It is also possible to adjust compilation settings for the SmartRedis library.
By default, the library compiles in an optimized build (``Release``), but debug builds
with full symbols (``Debug``) can be created as can debug builds with extensions enabled
for code coverage metrics (``Coverage``; this build type is only available with GNU
compilers). Similar to configuring a link type, selecting the build mode can be done
via a variable supplied to make:

.. code-block:: bash

    cd SmartRedis
    # Release build
    make lib BUILD_TYPE=Release # Default
    # Debug build
    make lib BUILD_TYPE=Debug
    # Code coverage build
    make lib BUILD_TYPE=Coverage

Finally, it is possible to build SmartRedis to include Python and/or Fortran support
(both are omitted by default):

.. code-block:: bash

    cd SmartRedis
    # Build support for Python
    make lib BUILD_PYTHON=ON
    # Build support for Fortran
    make lib BUILD_FORTRAN=ON # equivalent to make lib-with-fortran
    # Build support for Python and Fortran
    make lib BUILD_PYTHON=ON BUILD_FORTRAN=ON # or make lib-with-fortran BUILD_PYTHON=ON

Any combination of the build mode, link type, and Fortran/Python support
settings is supported For example, a statically linked debug build with Python
support may be achieved via the following command:

.. code-block:: bash

    cd SmartRedis
    make lib LINK_TYPE=Static BUILD_TYPE=Debug BUILD_PYTHON=ON

The LINK_TYPE, BUILD_TYPE, BUILD_PYTHON, and BUILD_FORTRAN variables are fully supported for all
test and build targets in the Makefile.

Fortran support is built in a secondary library ``libsmartredis-fortran``.  As
with the main library, the file extension is dependent on the link type, ``.so``
or ``.a``.  All libraries will be located in the ``install/lib`` folder.


Additional make variables are described in the ``help`` make target:

.. code-block:: bash

    cd SmartRedis
    make help

Adding SmartRedis as a CMake ExternalProject
--------------------------------------------

Most applications should be able to incorporate the following into their
``CMakeLists.txt`` to

.. code-block:: cmake

    include(ExternalProject)
    ExternalProject_Add(smartredis
        GIT_REPOSITORY https://github.com/CrayLabs/SmartRedis.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/external
                   -DBUILD_FORTRAN=on # For Fortran applications
        PREFIX ${CMAKE_BINARY_DIR}/external
    )
    ExternalProject_Get_Property(smartredis binary_dir source_dir)

    add_library(libsmartredis SHARED IMPORTED)
    add_dependencies(libsmartredis smartredis)
    set_target_properties(libsmartredis PROPERTIES
        IMPORTED_LOCATION ${binary_dir}/libsmartredis.so
        INTERFACE_INCLUDE_DIRECTORIES  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/external/include}>

    # Optional, only for Fortran applications
    add_library(libsmartredis-fortran SHARED IMPORTED)
    add_dependencies(libsmartredis-fortran smartredis)
    set_target_properties(libsmartredis PROPERTIES
        IMPORTED_LOCATION ${binary_dir}/libsmartredis-fortran.so
        INTERFACE_INCLUDE_DIRECTORIES  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/external/include}>

    # ... define the example_target executable here

    target_include_directories(example_target PRIVATE ${CMAKE_BINARY_DIR}/external/include)
    target_link_libraries(example_target libsmartredis)
    # Optional, only for Fortran applcations
    target_link_libraries(example_target libsmartredis-fortran)


Linking instructions using compiler flags
-----------------------------------------

For applications which use pre-defined compiler flags for compilation, the
following flags should be included for the preprocessor

.. code-block:: text

    -I/path/to/smartredis/install/include

The linking flags will differ slightly whether the Fortran client library needs
to be included. If so, be sure that you ran ``make lib-with-fortran`` (or ``make
lib BUILD_FORTRAN=ON``) and include the SmartRedis fortran library via the
following flags:

.. code-block:: text

    -L/path/to/smartredis/install/lib -lsmartredis [-lsmartredis-fortran]

.. note::

    Fortran applications need to link in both ``smartredis-fortran`` and
    ``smartredis`` libraries whereas C/C++ applications require only
    ``smartredis``.

.. warning::

    For static linking, due to limitations with CMake, users should also
    add the following flags ``-lhiredis -lredis++`` to the link command