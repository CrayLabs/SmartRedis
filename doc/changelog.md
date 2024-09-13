# Changelog

### Development branch

To be released at some future point in time

Description

-   Include algorithm import in rediscluster for gcc-14 and
    updated github artifact version
-   Touch-up outdated information in README.md
-   Update codecov to v4.5.0 for github actions
-   Remove broken oss.redis.com URLs from documentation
-   Add option to allow SmartRedis Fortran library to retain the
    path to the main client library
-   Update examples and tests to use find_package(smartredis)
-   Generate config files necessary to allow CMake projects to add
    SmartRedis via find_package
-   Allow users to specify install location of SmartRedis libraries
-   Streamline compilation of SmartRedis dependencies
-   Pin NumPy version to 1.x

Detailed Notes

-   Include algorithm import in rediscluster.h to satisfy
    gcc-14 compilation error and update github actions to
    upload-artifact@v3
    ([PR505](https://github.com/CrayLabs/SmartRedis/pull/505))
-   Update links to install documentation and remove outdated version
    numbers in the README.md
    ([PR501](https://github.com/CrayLabs/SmartRedis/pull/501))
-   Update codecov to v4.5.0 for github actions
    ([PR502](https://github.com/CrayLabs/SmartRedis/pull/502))
-   As part of this cleanup, some behaviors of how the libraries were
    named have been removed. The testing suite now distinguishes between
    various build types (e.g. Debug, Coverage, etc.) by specifying the
    ``CMAKE_INSTALL_PREFIX`` instead of appending it as part of the name
    of the library itself.
    ([PR497](https://github.com/CrayLabs/SmartRedis/pull/497))
-   The SmartRedis Fortran library now by default will retain the path
    to the SmartRedis C/C++ library. This should avoid occasional problems
    where users were getting "library not found" errors if they had moved
    libraries post-installation
    ([PR497](https://github.com/CrayLabs/SmartRedis/pull/497))
-   All the examples and tests now use the ``find_package`` functionality
    to setup linking flags
    ([PR497](https://github.com/CrayLabs/SmartRedis/pull/497))
-   The install process now generates package configuration files for
    the C/C++ SmartRedis library and the Fortran SmartRedis library.
    Users can use the ``find_package()`` command in their CMakeLists.txt
    to setup the linking and include flags automatically
    ([PR497](https://github.com/CrayLabs/SmartRedis/pull/497))
-   The CMakeLists.txt for SmartRedis now includes the install commands
    which allow users to specify the specific install prefix to install
    the SmartRedis libraries, header files, and Fortran .mod files
    ([PR497](https://github.com/CrayLabs/SmartRedis/pull/497))
-   hiredis, redis++, and pybind are now retrieved and installed
    in `CMakeLists.txt` instead of in the Makefile. This decouples the
    user-facing side of SmartRedis from the Makefile, which now can be
    used pureley as a convenient interface to compile SmartRedis with various
    options and coordinate testing
    ([PR497](https://github.com/CrayLabs/SmartRedis/pull/497))
-   The new major version release of Numpy is incompatible with modules
    compiled against Numpy 1.x. For both SmartSim and SmartRedis we request a
    1.x version of numpy. This is needed in SmartSim because some of the downstream
    dependencies request NumPy.
    ([PR498](https://github.com/CrayLabs/SmartRedis/pull/498))
-   Ensure errors raised from client include details

### 0.5.3

Released on 14 May 2024

Description

-   Improve client error logging
-   Fix pylint regression error
-   Fix build wheel error
-   Fix header styling issue
-   Correct changelog indention
-   Automate the creation of release notes
-   Auto-post release PR to develop from master
-   Upgrade ubuntu to 22.04 and gcc to 11
-   Drop Python 3.8 support
-   Fix C++ cosmetic defects leading to compiler warnings
-   Re-enable SR_PEDANTIC for the Makefile targets
-   Enforce changelog updates
-   Removed unused TensorBase constructor parameter
-   Remove unused parameter in internal redis cluster method
-   Enforce matching TensorType for DataSet::unpack_tensor()
-   Update CI for Intel suite
-   Add socket time out environment variable
-   Fix inconsistency in C-API ConfigOptions is_configured() parameters
-   Bump redis dep to 7.2.4
-   Fix widths field for list-table in install documentation
-   Remove a vestigial requirements.txt file

Detailed Notes

-   Ensure errors raised from client include details
    ([PR485](https://github.com/CrayLabs/SmartRedis/pull/485))
-   Pin pylint to fix regression error
    ([PR492](https://github.com/CrayLabs/SmartRedis/pull/492))
-   Add cstdint import to fix ubuntu with gcc wheel build
    ([PR491](https://github.com/CrayLabs/SmartRedis/pull/491))
-   Incorrect lineup of the changelog page index. This fixes the header
    sizes to avoid this issue.
    ([PR489](https://github.com/CrayLabs/SmartRedis/pull/489))
-   After converting from rst to md, readthedocs began throwing indention
    errors in old release info. This fixes the styling.
    ([PR488](https://github.com/CrayLabs/SmartRedis/pull/488))
-   Add a configuration file to the root of .github/ to configure
    the generated release notes.
    ([PR487](https://github.com/CrayLabs/SmartRedis/pull/487))
-   Add to github release workflow to auto generate a pull request from master
    into develop for release.
    ([PR486](https://github.com/CrayLabs/SmartRedis/pull/486))
-   After dropping support for Python 3.8, ubuntu and gcc need to be
    upgraded. ([PR484](https://github.com/CrayLabs/SmartRedis/pull/484))
-   Python 3.8 is reaching its end-of-life in October, 2024, so it will
    no longer continue to be supported.
    ([PR482](https://github.com/CrayLabs/SmartRedis/pull/482))
-   Fixes some mainly cosmetic defects in the C++ client that were
    leading to warnings when pedantic compiler flags were enabled
    ([PR476](https://github.com/CrayLabs/SmartRedis/pull/476))
-   Re-enable SR_PEDANTIC for the [test-lib]{.title-ref} and
    [test-lib-with-fortran]{.title-ref} Makefile targets
    ([PR476](https://github.com/CrayLabs/SmartRedis/pull/476))
-   Add Github Actions workflow that checks if changelog is edited on
    pull requests into develop.
    ([PR480](https://github.com/CrayLabs/SmartRedis/pull/480))
-   The TensorBase constructor SRMemoryLayout parameter was removed
    because it was not used. It is not needed as a member variable
    because all Tensor\<T\> objects store internal representations in
    contiguous memory.
    ([PR479](https://github.com/CrayLabs/SmartRedis/pull/479))
-   Client::unpack_tensor() enforces that the user-provided TensorType
    matches the known tensor type. Now DataSet::unpack_tensor() enforces
    the same condition.
    ([PR478](https://github.com/CrayLabs/SmartRedis/pull/478))
-   Removes an unused parameter in the
    RedisCluster::\_get_model_script_db() method.
    ([PR477](https://github.com/CrayLabs/SmartRedis/pull/477))
-   Version numbers changed for the Intel Compiler chain that lead to
    the C and C++ compilers not being available. Now, the entirety of
    the Base and HPC kits are installed to ensure consistent versions.
    ([PR475](https://github.com/CrayLabs/SmartRedis/pull/475))
-   Add the socket timeout parameter as a user-configurable option via
    environment variables.
    ([PR474](https://github.com/CrayLabs/SmartRedis/pull/474))
-   Fix an inconsistency in the C-API ConfigOptions is_configured()
    parameter names.
    ([PR471](https://github.com/CrayLabs/SmartRedis/pull/471))
-   Fix an issue where incorrect compiler flags are defined and result
    in build failures due to the redis_fstat macro.
    ([PR470](https://github.com/CrayLabs/SmartRedis/pull/470))
-   Fix wrong widths value which was preventing table from displaying.
    ([PR468](https://github.com/CrayLabs/SmartRedis/pull/468))
-   The requirements.txt file is unused, therefore removing.
    ([PR462](https://github.com/CrayLabs/SmartRedis/pull/462))

### 0.5.2

Released on February 16, 2024

Description

-   Fixed bug which was sending tensors to the database twice (Python
    Client)

Detailed Notes

-   A previous bug fix for the Python client which addressed a problem
    when sending numpy views inadvertently kept the original put_tensor
    call in place. This essentially doubles the cost of the operation.
    ([PR464](https://github.com/CrayLabs/SmartRedis/pull/464))

### 0.5.1

Released on February 15, 2024

Description

-   Fix bug when sending an array view
-   Add concurrency groups for Github Action testing
-   Update license to include 2024
-   Increase build space for Github Actions
-   Update README python versions
-   Expose Typehints
-   Update supported python versions \[Add 3.11, remove 3.7\]
-   Tweak the build system to enable building SmartRedis with Nvidia\'s
    NVHPC toolchain
-   Improvements/upgrades to the container used for Github actions
-   Code updates to avoid compiler warnings
-   Added developer documentation on how to run a single test case and
    eliminated duplicative environment variables
-   Resolve a linting issue with pybind-to-python error propagation
-   Use mutable fields to enable Dataset get methods that store memory
    to be marked const

Detailed Notes

-   Detect whether the tensor the user is sending is a view and if so,
    make an explicit copy.
    ([PR453](https://github.com/CrayLabs/SmartRedis/pull/453))
-   Add support to concurrency groups in the [run_tests]{.title-ref}
    workflow. ([PR456](https://github.com/CrayLabs/SmartRedis/pull/456))
-   Update license to include 2024.
    ([PR454](https://github.com/CrayLabs/SmartRedis/pull/454))
-   Add new Github Action that removes unneeded packages and resizes the
    root disk space.
    ([PR455](https://github.com/CrayLabs/SmartRedis/pull/455))
-   Update developer documentation to reflect newly supported versions
    of Python ([PR450](https://github.com/CrayLabs/SmartRedis/pull/450))
    ([PR452](https://github.com/CrayLabs/SmartRedis/pull/452))
-   Add and ship [py.typed]{.title-ref} marker to expose inline type
    hints ([PR451](https://github.com/CrayLabs/SmartRedis/pull/451))
-   Deprecate support for Python 3.7 by removing from the allowed Python
    versions ([PR450](https://github.com/CrayLabs/SmartRedis/pull/450))
-   Update Python package dependencies to add support for Python 3.11
    ([PR450](https://github.com/CrayLabs/SmartRedis/pull/450))
-   Change the order of arguments in our MakeFile to ensure that all
    dependencies are compiled with GCC
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   Add new user-configurable parameters DEP_CC, DEP_CXX to control
    which compiler is used to build dependencies
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   Ameliorate some compiler warnings related that were flagged in GCC
    12 (unreachable code blocks, signed/unsigned mismatches)
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   CI/CD: Bump the container version used in Github Actions Ubuntu
    22.04 to be able to start testing GCC 12
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   CI/CD: Bump the versions of GCC used in testing to the currently
    maintained versions
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   CI/CD: Add NVHPC to the testing matrix
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   CI/CD: Test the shared/static compilations and examples with all
    compilers ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   CI/CD: Compile Redis and RedisAI and use those versions in testing
    instead of extracting from a container
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   CI/CD: Bump the version of Redis used in testing to 7.0.5, the same
    version as we use with SmartSim
    ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   CI/CD: Pin the Torch version to 1.11.0, the same as supported in
    SmartSim ([PR448](https://github.com/CrayLabs/SmartRedis/pull/448))
-   Added developer documentation on how to run a single test case with
    the new test/build system and eliminated use of
    SMARTREDIS_TEST_DEVICE and SMARTREDIS_TEST_CLUSTER environment
    variables ([PR445](https://github.com/CrayLabs/SmartRedis/pull/445))
-   Resolve a linting issue with pybind-to-python error propagation by
    changing import format and narrowing the lookup of pybind error
    names to the error module
    ([PR444](https://github.com/CrayLabs/SmartRedis/pull/444))
-   Use mutable fields to enable Dataset get methods that store memory
    to be marked const
    ([PR443](https://github.com/CrayLabs/SmartRedis/pull/443))

### 0.5.0

Released on December 18, 2023

Description

-   Unpin the Intel Fortran compiler in CI/CD
-   Added a missing space in an error message
-   Improved consistency of namespace declarations for C++ pybind
    interface
-   Improved const correctness of C++ Client
-   Improved const correctness of C++ Dataset
-   Updated documentation
-   Added test cases for all Client construction parameter combinations
-   Centralized dependency tracking to setup.cfg
-   Improved robustness of Python client construction
-   Updated Client and Dataset documentation
-   Expanded list of allowed characters in the SSDB address
-   Added coverage to SmartRedis Python API functions
-   Improved responsiveness of library when attempting connection to
    missing backend database
-   Moved testing of examples to on-commit testing in CI/CD pipeline
-   Added name retrieval function to the DataSet object
-   Updated RedisAI version used in post-commit check-in testing in
    Github pipeline
-   Allow strings in Python interface for Client.run_script,
    Client.run_script_multiGPU
-   Improved support for model execution batching
-   Added support for model chunking
-   Updated the third-party RedisAI component
-   Updated the third-party lcov component
-   Add link to contributing guidelines
-   Added link to contributing guidelines
-   Added support for multiple backend databases via a new Client
    constructor that accepts a ConfigOptions object

Detailed Notes

-   Unpin the Intel Fortran compiler in CI/CD. This requires running the
    compiler setup script twice, once for Fortran and once for other
    languages, since they\'re on different releases
    ([PR436](https://github.com/CrayLabs/SmartRedis/pull/436))
-   Added a missing space in an error message
    ([PR435](https://github.com/CrayLabs/SmartRedis/pull/435))
-   Made the declaration of the py namespace in py\*.h consistently
    outside the SmartRedis namespace declaration
    ([PR434](https://github.com/CrayLabs/SmartRedis/pull/434))
-   Fields in several C++ API methods are now properly marked as const
    ([PR430](https://github.com/CrayLabs/SmartRedis/pull/430))
-   The Dataset add_tensor method is now const correct, as are all
    internal the methods it calls
    ([PR427](https://github.com/CrayLabs/SmartRedis/pull/427))
-   Some broken links in the documentation were fixed, and the
    instructions to run the tests were updated
    ([PR423](https://github.com/CrayLabs/SmartRedis/pull/423))
-   Added test cases for all Client construction parameter combinations
    ([PR422](https://github.com/CrayLabs/SmartRedis/pull/422))
-   Merged dependency lists from requirements.txt and
    requirements-dev.txt into setup.cfg to have only one set of
    dependencies going forward
    ([PR420](https://github.com/CrayLabs/SmartRedis/pull/420))
-   Improved robustness of Python client construction by adding
    detection of invalid kwargs
    ([PR419](https://github.com/CrayLabs/SmartRedis/pull/419)),
    ([PR421](https://github.com/CrayLabs/SmartRedis/pull/421))
-   Updated the Client and Dataset API documentation to clarify which
    interacts with the backend db
    ([PR416](https://github.com/CrayLabs/SmartRedis/pull/416))
-   The SSDB address can now include \'-\' and \'\_\' as special
    characters in the name. This gives users more options for naming the
    UDS socket file
    ([PR415](https://github.com/CrayLabs/SmartRedis/pull/415))
-   Added tests to increase Python code coverage
-   Employed a Redis++ ConnectionsObject in the connection process to
    establish a TCP timeout of 100ms during connection attempts
    ([PR413](https://github.com/CrayLabs/SmartRedis/pull/413))
-   Moved testing of examples to on-commit testing in CI/CD pipeline
    ([PR412](https://github.com/CrayLabs/SmartRedis/pull/412))
-   Added a function to the DataSet class and added a test
-   Updated RedisAI version used in post-commit check-in testing in
    Github pipeline to a version that supports fetch of model chunking
    size ([PR408](https://github.com/CrayLabs/SmartRedis/pull/408))
-   Allow users to pass single keys for the inputs and outputs
    parameters as a string for Python run_script and run_script_multigpu
-   Exposed access to the Redis.AI MINBATCHTIMEOUT parameter, which
    limits the delay in model execution when trying to accumulate
    multiple executions in a batch
    ([PR406](https://github.com/CrayLabs/SmartRedis/pull/406))
-   Models will now be automatically chunked when sent to/received from
    the backed database. This allows use of models greater than 511MB in
    size. ([PR404](https://github.com/CrayLabs/SmartRedis/pull/404))
-   Updated from RedisAI v1.2.3 (test target)/v1.2.4 and v1.2.5 (CI/CD
    pipeline) to v1.2.7
    ([PR402](https://github.com/CrayLabs/SmartRedis/pull/402))
-   Updated lcov from version 1.15 to 2.0
    ([PR396](https://github.com/CrayLabs/SmartRedis/pull/396))
-   Create CONTRIBUTIONS.md file that points to the contribution
    guideline for both SmartSim and SmartRedis
    ([PR395](https://github.com/CrayLabs/SmartRedis/pull/395))
-   Migrated to ConfigOptions-based Client construction, adding multiple
    database support
    ([PR353](https://github.com/CrayLabs/SmartRedis/pull/353))

### 0.4.2

Released on September 13, 2023

Description

-   Reduced number of suppressed lint errors
-   Expanded documentation of aggregation lists
-   Updated third-party software dependencies to current versions
-   Updated post-merge tests in CI/CD to work with new test system
-   Enabled static builds of SmartRedis
-   Improve robustness of test runs
-   Fixed installation link
-   Updated supported languages documentation
-   Removed obsolete files
-   Added pylint to CI/CD pipeline and mitigate existing errors
-   Improved clustered redis initialization

Detailed Notes

-   Refactor factory for ConfigOptions to avoid using protected member
    outside an instance
    ([PR393](https://github.com/CrayLabs/SmartRedis/pull/393))
-   Added a new advanced topics documentation page with a section on
    aggregation lists
    ([PR390](https://github.com/CrayLabs/SmartRedis/pull/390))
-   Updated pybind (2.10.3 =\> 2.11.1), hiredis (1.1.0 =\> 1.2.0), and
    redis++ (1.3.5 =\> 1.3.10) dependencies to current versions
    ([PR389](https://github.com/CrayLabs/SmartRedis/pull/389))
-   Post-merge tests in CI/CD have been updated to interface cleanly
    with the new test system that was deployed in the previous release
    ([PR388](https://github.com/CrayLabs/SmartRedis/pull/388))
-   Static builds of SmartRedis can now work with Linux platforms.
    Fortran is tested with GNU, PGI, Intel compilers
    ([PR386](https://github.com/CrayLabs/SmartRedis/pull/386))
-   Preserve the shell output of test runs while making sure that server
    shutdown happens unconditionally
    ([PR381](https://github.com/CrayLabs/SmartRedis/pull/381))
-   Fix incorrect link to installation documentation
    ([PR380](https://github.com/CrayLabs/SmartRedis/pull/380))
-   Update language support matrix in documentation to reflect updates
    from the last release
    ([PR379](https://github.com/CrayLabs/SmartRedis/pull/379))
-   Fix typo causing startup failure in utility script for unit tests
    ([PR378](https://github.com/CrayLabs/SmartRedis/pull/378))
-   Update pylint configuration and version, mitigate most errors,
    execute in CI/CD pipeline
    ([PR371](https://github.com/CrayLabs/SmartRedis/pull/371),
    [PR382](https://github.com/CrayLabs/SmartRedis/pull/382))
-   Deleted obsolete build and testing files that are no longer needed
    with the new build and test system
    ([PR366](https://github.com/CrayLabs/SmartRedis/pull/366))
-   Reuse existing redis connection when mapping the Redis cluster
    ([PR364](https://github.com/CrayLabs/SmartRedis/pull/364))

### 0.4.1

Released on July 5, 2023

Description

This release revamps the build and test systems for SmartRedis as well
as improving compatibility with different Fortran compilers and laying
the groundwork for future support for interacting with multiple
concurrent backend databases:

-   Documentation improvements
-   Improved compatibility of type hints with third-party software
-   Added type hints to the Python interface layer
-   Add support for Python 3.10
-   Updated setup.py to work with the new build system
-   Remove unneeded method from Python SRObject class
-   Fixed a memory leak in the C layer
-   Revamp SmartRedis test system
-   Remove debug output in pybind layer
-   Update Hiredis version to 1.1.0
-   Enable parallel build for the SmartRedis examples
-   Experimental support for Nvidia toolchain
-   Major revamp of build and test systems for SmartRedis
-   Refactor Fortran methods to return default logical kind
-   Update CI/CD tests to use a modern version of MacOS
-   Fix the spelling of the Dataset destructor\'s C interface (now
    DeallocateDataSet)
-   Update Redis++ version to 1.3.8
-   Refactor third-party software dependency installation
-   Add pip-install target to Makefile to automate this process going
    forward (note: this was later removed)
-   Added infrastructure for multiDB support

Detailed Notes

-   Assorted updates and clarifications to the documentation
    ([PR367](https://github.com/CrayLabs/SmartRedis/pull/367))
-   Turn [ParamSpec]{.title-ref} usage into forward references to not
    require [typing-extensions]{.title-ref} at runtime
    ([PR365](https://github.com/CrayLabs/SmartRedis/pull/365))
-   Added type hints to the Python interface layer
    ([PR361](https://github.com/CrayLabs/SmartRedis/pull/361))
-   List Python 3.10 support and loosen PyTorch requirement to allow for
    versions support Python 3.10
    ([PR360](https://github.com/CrayLabs/SmartRedis/pull/360))
-   Streamlined setup.py to simplify Python install (PR359)
-   Remove from_pybind() from Python SRObject class as it\'s not needed
    and didn\'t work properly anyway
    ([PR358](https://github.com/CrayLabs/SmartRedis/pull/358))
-   Fixed memory leaked from the C layer when calling
    get_string_option()
    ([PR357](https://github.com/CrayLabs/SmartRedis/pull/357))
-   Major revamp to simplify use of SmartRedis test system, automating
    most test processes
    ([PR356](https://github.com/CrayLabs/SmartRedis/pull/356))
-   Remove debug output in pybind layer associated with put_dataset
    ([PR352](https://github.com/CrayLabs/SmartRedis/pull/352))
-   Updated to the latest version of Hiredis (1.1.0)
    ([PR351](https://github.com/CrayLabs/SmartRedis/pull/351))
-   Enable parallel build for the SmartRedis examples by moving utility
    Fortran code into a small static library
    ([PR349](https://github.com/CrayLabs/SmartRedis/pull/349))
-   For the NVidia toolchain only: Replaces the assumed rank feature of
    F2018 used in the Fortran client with assumed shape arrays, making
    it possible to compile SmartRedis with the Nvidia toolchain.
    ([PR346](https://github.com/CrayLabs/SmartRedis/pull/346))
-   Rework the build and test system to improve maintainability of the
    library. There have been several significant changes, including that
    Python and Fortran clients are no longer built by defaults and that
    there are Make variables that customize the build process. Please
    review the build documentation and `make help` to see all that has
    changed. ([PR341](https://github.com/CrayLabs/SmartRedis/pull/341))
-   Many Fortran routines were returning logical kind = c_bool which
    turns out not to be the same default kind of most Fortran compilers.
    These have now been refactored so that users need not import
    [iso_c\_binding]{.title-ref} in their own applications
    ([PR340](https://github.com/CrayLabs/SmartRedis/pull/340))
-   Update MacOS version in CI/CD tests from 10.15 to 12.0
    ([PR339](https://github.com/CrayLabs/SmartRedis/pull/339))
-   Correct the spelling of the C DataSet destruction interface from
    DeallocateeDataSet to DeallocateDataSet
    ([PR338](https://github.com/CrayLabs/SmartRedis/pull/338))
-   Updated the version of Redis++ to v1.3.8 to pull in a change that
    ensures the redis++.pc file properly points to the generated
    libraries ([PR334](https://github.com/CrayLabs/SmartRedis/pull/334))
-   Third-party software dependency installation is now handled in the
    Makefile instead of separate scripts
-   New pip-install target in Makefile will be a dependency of the lib
    target going forward so that users don\'t have to manually pip
    install SmartRedis in the future
    ([PR330](https://github.com/CrayLabs/SmartRedis/pull/330))
-   Added ConfigOptions class and API, which will form the backbone of
    multiDB support
    ([PR303](https://github.com/CrayLabs/SmartRedis/pull/303))

### 0.4.0

Released on April 12, 2023

Description

This release provides a variety of features to improve usability and
debugging of the SmartRedis library, notably including Unix domain
socket support, logging, the ability to print a textual representation
of a string or dataset, dataset inspection, documentation updates, fixes
to the multi-GPU support, and much more:

-   Prepare 0.4.0 release
-   Disable codecov CI tests
-   Improved error message in to_string methods in C interface
-   Streamlined PyBind interface layer
-   Updated Python API documentation
-   Streamlined C interface layer
-   Improved performance of get, put, and copy dataset methods
-   Fix a bug which prevented multi-GPU model set in some cases
-   Streamline pipelined execution of tasks for backend database
-   Enhance code coverage to include all 4 languages supported by
    SmartRedis
-   Fix a bug which resulted in wrong key prefixing when retrieving
    aggregation lists in ensembles
-   Correct assorted API documentation errors and omissions
-   Improve documentation of exception handling in Redis server classes
-   Improve error handling for setting of scripts and models
-   Add support to inspect the dimensions of a tensor via
    get_tensor_dims()
-   Split dataset prefixing control from use_tensor_ensemble_prefix() to
    use_dataset_ensemble_prefix()
-   Update to the latest version of redis-plus-plus
-   Update to the latest version of PyBind
-   Change documentation theme to sphinx_book_theme and fix doc strings
-   Add print capability for Client and DataSet
-   Add support for inspection of tensors and metadata inside datasets
-   Add support for user-directed logging for Python clients, using
    Client, Dataset, or LogContext logging methods
-   Add support for user-directed logging for C and Fortran clients
    without a Client or Dataset context
-   Additional error reporting for connections to and commands run
    against Redis databases
-   Improved error reporting capabilities for Fortran clients
-   Python error messages from SmartRedis contain more information
-   Added logging functionality to the SmartRedis library
-   A bug related to thread pool initialization was fixed.
-   This version adds new functionality in the form of support for Unix
    Domain Sockets.
-   Fortran client can now be optionally built with the rest of the
    library
-   Initial support for dataset conversions, specifically Xarray.

Detailed Notes

-   Update docs and version numbers in preparation for version 0.4.0.
    Clean up duplicate marking of numpy dependency
    ([PR321](https://github.com/CrayLabs/SmartRedis/pull/321))
-   Remove codecov thresholds to avoid commits being marked as
    \'failed\' due to coverage variance
    ([PR317](https://github.com/CrayLabs/SmartRedis/pull/317))
-   Corrected the error message in to_string methods in C interface to
    not overwrite the returned error message and to name the function
    ([PR320](https://github.com/CrayLabs/SmartRedis/pull/320))
-   Streamlined PyBind interface layer to reduce repetitive boilerplate
    code ([PR315](https://github.com/CrayLabs/SmartRedis/pull/315))
-   Updated Python API summary table to include new methods
    ([PR313](https://github.com/CrayLabs/SmartRedis/pull/313))
-   Streamlined C interface layer to reduce repetitive boilerplate code
    ([PR312](https://github.com/CrayLabs/SmartRedis/pull/312))
-   Leveraged Redis pipelining to improve performance of get, put, and
    copy dataset methods
    ([PR311](https://github.com/CrayLabs/SmartRedis/pull/311))
-   <Redis::set_model_multigpu>() will now upload the correct model to
    all GPUs ([PR310](https://github.com/CrayLabs/SmartRedis/pull/310))
-   RedisCluster::\_run_pipeline() will no longer unconditionally apply
    a retry wait before returning
    ([PR309](https://github.com/CrayLabs/SmartRedis/pull/309))
-   Expand code coverage to all four languages and make the CI/CD more
    efficent ([PR308](https://github.com/CrayLabs/SmartRedis/pull/308))
-   An internal flag was set incorrectly, it resulted in wrong key
    prefixing when accessing (retrieving or querying) lists created in
    ensembles ([PR306](https://github.com/CrayLabs/SmartRedis/pull/306))
-   Corrected a variety of Doxygen errors and omissions in the API
    documentation
    ([PR305](https://github.com/CrayLabs/SmartRedis/pull/305))
-   Added throw documentation for exception handling in redis.h,
    redisserver.h, rediscluster.h
    ([PR301](https://github.com/CrayLabs/SmartRedis/pull/301))
-   Added error handling for a rare edge condition when setting scripts
    and models
    ([PR300](https://github.com/CrayLabs/SmartRedis/pull/300))
-   Added support to inspect the dimensions of a tensor via new
    get_tensor_dims() method
    ([PR299](https://github.com/CrayLabs/SmartRedis/pull/299))
-   The use_tensor_ensemble_prefix() API method no longer controls
    whether datasets are prefixed. A new API method,
    use_dataset_ensemble_prefix() now manages this.
    ([PR298](https://github.com/CrayLabs/SmartRedis/pull/298))
-   Updated from redis-plus-plus v1.3.2 to v1.3.5
    ([PR296](https://github.com/CrayLabs/SmartRedis/pull/296))
-   Updated from PyBind v2.6.2 to v2.10.3
    ([PR295](https://github.com/CrayLabs/SmartRedis/pull/295))
-   Change documentation theme to sphinx_book_theme to match SmartSim
    documentation theme and fix Python API doc string errors
    ([PR294](https://github.com/CrayLabs/SmartRedis/pull/294))
-   Added print capability for Client and DataSet to give details
    diagnostic information for debugging
    ([PR293](https://github.com/CrayLabs/SmartRedis/pull/293))
-   Added support for retrieval of names and types of tensors and
    metadata inside datasets
    ([PR291](https://github.com/CrayLabs/SmartRedis/pull/291))
-   Added support for user-directed logging for Python clients via
    {Client, Dataset, LogContext}.{log_data, log_warning, log_error}
    methods ([PR289](https://github.com/CrayLabs/SmartRedis/pull/289))
-   Added support for user-directed logging without a Client or Dataset
    context to C and Fortran clients via \_string() methods
    ([PR288](https://github.com/CrayLabs/SmartRedis/pull/288))
-   Added logging to capture transient errors that arise in the \_run()
    and \_connect() methods of the Redis and RedisCluster classes
    ([PR287](https://github.com/CrayLabs/SmartRedis/pull/287))
-   Tweak direct testing of Redis and RedisCluster classes
    ([PR286](https://github.com/CrayLabs/SmartRedis/pull/286))
-   Resolve a disparity in the construction of Python client and
    database classes
    ([PR285](https://github.com/CrayLabs/SmartRedis/pull/285))
-   Fortran clients can now access error text and source location
    ([PR284](https://github.com/CrayLabs/SmartRedis/pull/284))
-   Add exception location information from CPP code to Python
    exceptions
    ([PR283](https://github.com/CrayLabs/SmartRedis/pull/283))
-   Added client activity and manual logging for developer use
    ([PR281](https://github.com/CrayLabs/SmartRedis/pull/281))
-   Fix thread pool error
    ([PR280](https://github.com/CrayLabs/SmartRedis/pull/280))
-   Update library linking instructions and update Fortran tester build
    process ([PR277](https://github.com/CrayLabs/SmartRedis/pull/277))
-   Added [add_metadata_for_xarray]{.title-ref} and
    [transform_to_xarray]{.title-ref} methods in
    [DatasetConverter]{.title-ref} class for initial support with Xarray
    ([PR262](https://github.com/CrayLabs/SmartRedis/pull/262))
-   Change Dockerfile to use Ubuntu 20.04 LTS image
    ([PR276](https://github.com/CrayLabs/SmartRedis/pull/276))
-   Implemented support for Unix Domain Sockets, including
    refactorization of server address code, test cases, and check-in
    tests. ([PR252](https://github.com/CrayLabs/SmartRedis/pull/252))
-   A new make target [make lib-with-fortran]{.title-ref} now compiles
    the Fortran client and dataset into its own library which
    applications can link against
    ([PR245](https://github.com/CrayLabs/SmartRedis/pull/245))

### 0.3.1

Released on June 24, 2022

Description

Version 0.3.1 adds new functionality in the form of DataSet aggregation
lists for pipelined retrieval of data, convenient support for multiple
GPUs, and the ability to delete scripts and models from the backend
database. It also introduces multithreaded execution for certain tasks
that span multiple shards of a clustered database, and it incorporates a
variety of internal improvements that will enhance the library going
forward.

Detailed Notes

-   Implemented DataSet aggregation lists in all client languages, for
    pipelined retrieval of data across clustered and non-clustered
    backend databases.
    ([PR258](https://github.com/CrayLabs/SmartRedis/pull/258))
    ([PR257](https://github.com/CrayLabs/SmartRedis/pull/257))
    ([PR256](https://github.com/CrayLabs/SmartRedis/pull/256))
    ([PR248](https://github.com/CrayLabs/SmartRedis/pull/248)) New
    commands are:
    -   append_to_list()
    -   delete_list()
    -   copy_list()
    -   rename_list()
    -   get_list_length()
    -   poll_list_length()
    -   poll_list_length_gte()
    -   poll_list_length_lte()
    -   get_datasets_from_list()
    -   get_dataset_list_range()
    -   use_list_ensemble_prefix()
-   Implemented multithreaded execution for parallel dataset list
    retrieval on clustered databases. The number of threads devoted for
    this purpose is controlled by the new environment variable
    SR_THERAD_COUNT. The value defaults to 4, but may be any positive
    integer or special value zero, which will cause the SmartRedis
    runtime to allocate one thread for each available hardware context.
    ([PR251](https://github.com/CrayLabs/SmartRedis/pull/251))
    ([PR246](https://github.com/CrayLabs/SmartRedis/pull/246))
-   Augmented support for GPUs by implementing multi-GPU convenience
    functions for all client languages.
    ([PR254](https://github.com/CrayLabs/SmartRedis/pull/254))
    ([PR250](https://github.com/CrayLabs/SmartRedis/pull/250))
    ([PR244](https://github.com/CrayLabs/SmartRedis/pull/244)) New
    commands are:
    -   set_model_from_file_multigpu()
    -   set_model_multigpu()
    -   set_script_from_file_multigpu()
    -   set_script_multigpu()
    -   run_model_multigpu()
    -   run_script_multigpu()
    -   delete_model_multigpu()
    -   delete_script_multigpu()
-   Added API calls for all clients to delete models and scripts from
    the backend database.
    ([PR240](https://github.com/CrayLabs/SmartRedis/pull/240)) New
    commands are:
    -   delete_script()
    -   delete_model()
-   Updated the use of backend RedisAI API calls to discontinue use of
    deprecated methods for model selection (AI.MODELSET) and execution
    (AI.MODELRUN) in favor of current methods AI.MODELSTORE and
    AI.MODELEXECUTE, respectively.
    ([PR234](https://github.com/CrayLabs/SmartRedis/pull/234))
-   SmartRedis will no longer call the C runtime method srand() to
    ensure that it does not interfere with random number generation in
    client code. It now uses a separate instance of the C++ random
    number generator.
    ([PR233](https://github.com/CrayLabs/SmartRedis/pull/233))
-   Updated the way that the Fortran enum_kind type defined in the
    fortran_c\_interop module is defined in order to better comply with
    Fortran standard and not interfere with GCC 6.3.0.
    ([PR231](https://github.com/CrayLabs/SmartRedis/pull/231))
-   Corrected the spelling of the word \"command\" in a few error
    message strings.
    ([PR221](https://github.com/CrayLabs/SmartRedis/pull/221))
-   SmartRedis now requires a CMake version 3.13 or later in order to
    utilize the add_link_options CMake command.
    ([PR217](https://github.com/CrayLabs/SmartRedis/pull/217))
-   Updated and improved the documentation of the SmartRedis library. In
    particular, a new SmartRedis Integration Guide provides an
    introduction to using the SmartRedis library and integrating it with
    existing software.
    ([PR261](https://github.com/CrayLabs/SmartRedis/pull/261))
    ([PR260](https://github.com/CrayLabs/SmartRedis/pull/260))
    ([PR259](https://github.com/CrayLabs/SmartRedis/pull/259))
    ([SSPR214](https://github.com/CrayLabs/SmartSim/pull/214))
-   Added clustered Redis testing to automated GitHub check-in testing.
    ([PR239](https://github.com/CrayLabs/SmartRedis/pull/239))
-   Updated the SmartRedis internal API for building commands for the
    backend database.
    ([PR223](https://github.com/CrayLabs/SmartRedis/pull/223)) This
    change should not be visible to clients.
-   The SmartRedis example code is now validated through the automated
    GitHub checkin process. This will help ensure that the examples do
    not fall out of date.
    ([PR220](https://github.com/CrayLabs/SmartRedis/pull/220))
-   Added missing copyright statements to CMakeLists.txt and the
    SmartRedis examples.
    ([PR219](https://github.com/CrayLabs/SmartRedis/pull/219))
-   Updated the C++ test coverage to ensure that all test files are
    properly executed when running \"make test\".
    ([PR218](https://github.com/CrayLabs/SmartRedis/pull/218))
-   Fixed an internal naming conflict between a local variable and a
    class member variable in the DataSet class.
    ([PR215](https://github.com/CrayLabs/SmartRedis/pull/215)) This
    should not be visible to clients.
-   Updated the internal documentation of methods in SmartRedis C++
    classes with the override keyword to improve compliance with the
    latest C++ standards.
    ([PR214](https://github.com/CrayLabs/SmartRedis/pull/214)) This
    change should not be visible to clients.
-   Renamed variables internally to more cleanly differentiate between
    names that are given to clients for tensors, models, scripts,
    datasets, etc., and the keys that are used when storing them in the
    backend database.
    ([PR213](https://github.com/CrayLabs/SmartRedis/pull/213)) This
    change should not be visible to clients.

### 0.3.0

Released on February 11, 2022

Description

-   Improve error handling across all SmartRedis clients
    ([PR159](https://github.com/CrayLabs/SmartRedis/pull/159))
    ([PR191](https://github.com/CrayLabs/SmartRedis/pull/191))
    ([PR199](https://github.com/CrayLabs/SmartRedis/pull/199))
    ([PR205](https://github.com/CrayLabs/SmartRedis/pull/205))
    ([PR206](https://github.com/CrayLabs/SmartRedis/pull/206))
    Includes changes to C and Fortran function prototypes that are
    not backwards compatible. Includes changes to error class names
    and enum type names that are not backwards compatible
-   Add `poll_dataset` functionality to all SmartRedis clients
    ([PR184](https://github.com/CrayLabs/SmartRedis/pull/184))
    Due to other breaking changes made in this release, applications
    using methods other than `poll_dataset` to check for the
    existence of a dataset should now use `poll_dataset`
-   Add environment variables to control client connection and command
    timeout behavior
    ([PR194](https://github.com/CrayLabs/SmartRedis/pull/194))
-   Add AI.INFO command to retrieve statistics on scripts and models via
    Python and C++ clients
    ([PR197](https://github.com/CrayLabs/SmartRedis/pull/197))
-   Create a Dockerfile for SmartRedis
    ([PR180](https://github.com/CrayLabs/SmartRedis/pull/180))
-   Update `redis-plus-plus` version to 1.3.2
    ([PR162](https://github.com/CrayLabs/SmartRedis/pull/162))
-   Internal client performance and API improvements
    ([PR138](https://github.com/CrayLabs/SmartRedis/pull/138))
    ([PR141](https://github.com/CrayLabs/SmartRedis/pull/141))
    ([PR163](https://github.com/CrayLabs/SmartRedis/pull/163))
    ([PR203](https://github.com/CrayLabs/SmartRedis/pull/203))
-   Expose Redis `FLUSHDB`, `CONFIG GET`, `CONFIG SET`, and `SAVE`
    commands to the Python client
    ([PR139](https://github.com/CrayLabs/SmartRedis/pull/139))
    ([PR160](https://github.com/CrayLabs/SmartRedis/pull/160))
-   Extend inverse CRC16 prefixing to all hash slots
    ([PR161](https://github.com/CrayLabs/SmartRedis/pull/161))
-   Improve backend dataset representation to enable performance
    optimization
    ([PR195](https://github.com/CrayLabs/SmartRedis/pull/195))
-   Simplify SmartRedis build proccess
    ([PR189](https://github.com/CrayLabs/SmartRedis/pull/189))
-   Fix zero-length array transfer in Fortran `convert_char_array_to_c`
    ([PR170](https://github.com/CrayLabs/SmartRedis/pull/170))
-   Add continuous integration for all SmartRedis tests
    ([PR165](https://github.com/CrayLabs/SmartRedis/pull/165))
    ([PR173](https://github.com/CrayLabs/SmartRedis/pull/173))
    ([PR177](https://github.com/CrayLabs/SmartRedis/pull/177))
-   Update SmartRedis docstrings
    ([PR200](https://github.com/CrayLabs/SmartRedis/pull/200))
    ([PR207](https://github.com/CrayLabs/SmartRedis/pull/207))
-   Update SmartRedis documentation and examples
    ([PR202](https://github.com/CrayLabs/SmartRedis/pull/202))
    ([PR208](https://github.com/CrayLabs/SmartRedis/pull/208))
    ([PR210](https://github.com/CrayLabs/SmartRedis/pull/210))

### 0.2.0

Released on August, 5, 2021

Description

-   Improved tensor memory management in the Python client
    ([PR70](https://github.com/CrayLabs/SmartRedis/pull/70))
-   Improved metadata serialization and removed protobuf dependency
    ([PR61](https://github.com/CrayLabs/SmartRedis/pull/61))
-   Added unit testing infrastructure for the C++ client
    ([PR96](https://github.com/CrayLabs/SmartRedis/pull/96))
-   Improve command execution fault handling
    ([PR65](https://github.com/CrayLabs/SmartRedis/pull/65))
    ([PR97](https://github.com/CrayLabs/SmartRedis/pull/97))
    ([PR105](https://github.com/CrayLabs/SmartRedis/pull/105))
-   Bug fixes ([PR52](https://github.com/CrayLabs/SmartRedis/pull/52))
    ([PR72](https://github.com/CrayLabs/SmartRedis/pull/72))
    ([PR76](https://github.com/CrayLabs/SmartRedis/pull/76))
    ([PR84](https://github.com/CrayLabs/SmartRedis/pull/84))
-   Added copy, rename, and delete tensor and DataSet commands in the
    Python client
    ([PR66](https://github.com/CrayLabs/SmartRedis/pull/66))
-   Upgrade to RedisAI 1.2.3
    ([PR101](https://github.com/CrayLabs/SmartRedis/pull/101))
-   Fortran and C interface improvements
    ([PR93](https://github.com/CrayLabs/SmartRedis/pull/93))
    ([PR94](https://github.com/CrayLabs/SmartRedis/pull/94))
    ([PR95](https://github.com/CrayLabs/SmartRedis/pull/95))
    ([PR99](https://github.com/CrayLabs/SmartRedis/pull/99))
-   Add Redis INFO command execution to the Python client
    ([PR83](https://github.com/CrayLabs/SmartRedis/pull/83))
-   Add Redis CLUSTER INFO command execution to the Python client
    ([PR105](https://github.com/CrayLabs/SmartRedis/pull/105))

### 0.1.1

Released on May 5, 2021

Description

-   Compiled client library build and install update to remove
    environment variables
    ([PR47](https://github.com/CrayLabs/SmartRedis/pull/47))
-   Pip install for Python client
    ([PR45](https://github.com/CrayLabs/SmartRedis/pull/45))

### 0.1.0

Released on April 1, 2021

Description

-   Initial 0.1.0 release of SmartRedis
