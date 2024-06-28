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
# Enable setting version in the project statement

if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

## Project definition for the SmartRedis
cmake_minimum_required(VERSION 3.13)
project(SmartRedis VERSION "0.5.3")

## Specify options for the SmartRedis project
option(BUILD_FORTRAN "Build the fortran client library" OFF)
option(BUILD_PYTHON  "Build the python module" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(PEDANTIC "Build with strictest compiler settings" OFF)
option(RETAIN_RPATH "Keep the path of the linked main SmartRedis library. Only applies to shared library builds" ON)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
endif()
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

## Cmake Modules
include(GNUInstallDirs)
include(ExternalProject)
if(BUILD_PYTHON)
    include(FetchContent)
endif()
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(EnableCoverage)

## Configure the remainder of the builder
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET default)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

if (BUILD_FORTRAN)
    enable_language(Fortran)
    set(CMAKE_Fortran_MODULE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/include")
endif()

if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    enable_coverage()
endif()

## Include external libraries
## Note: These ExternalProjects need to be installed into the
## SmartRedis install directory since there are variants of the
## header files that are chosen based on various configuration
## options. Any extra artifacts should be removed in the
## POST_BUILD step

# Add hiredis as an external project
ExternalProject_Add(hiredis
    GIT_REPOSITORY https://github.com/redis/hiredis.git
    GIT_TAG v1.2.0
    GIT_SHALLOW 1
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
    INSTALL_COMMAND make install
)

# Define hiredis as an external project
ExternalProject_Get_Property(hiredis source_dir binary_dir)
set(hiredis_source_dir ${source_dir})
set(hiredis_binary_dir ${binary_dir})

add_library(libhiredis STATIC IMPORTED)
add_dependencies(libhiredis hiredis)

# Set hiredis properties
set_target_properties(libhiredis PROPERTIES
    IMPORTED_LOCATION ${hiredis_binary_dir}/libhiredis.a
    INTERFACE_INCLUDE_DIRECTORIES ${hiredis_source_dir}
)

# Define redis++ as an external project
ExternalProject_Add(redis++
    GIT_REPOSITORY https://github.com/sewenew/redis-plus-plus.git
    GIT_TAG 1.3.10
    GIT_SHALLOW 1
    INSTALL_COMMAND make install PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_ARGS -DREDIS_PLUS_PLUS_BUILD_TEST=OFF
               -DREDIS_PLUS_PLUS_BUILD_SHARED=OFF
               -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
               -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    DEPENDS hiredis  # Add hiredis as a dependency
)

ExternalProject_Get_Property(redis++ source_dir binary_dir)
set(redis++_source_dir ${source_dir})
set(redis++_binary_dir ${binary_dir})


add_library(libredis++ STATIC IMPORTED)
add_dependencies(libredis++ redis++)

set_target_properties(libredis++ PROPERTIES
    IMPORTED_LOCATION ${redis++_binary_dir}/libredis++.a
    INTERFACE_INCLUDE_DIRECTORIES ${redis++_source_dir}
)

# Add pybind11
if(BUILD_PYTHON)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG        v2.11.1
    )
endif(BUILD_PYTHON)

# For now, we only support Pedantic on the main library build.
if (PEDANTIC)
    if(
        (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "NVHPC")
    )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic -Wextra -Wno-unused-parameter -Wno-unused-variable")
	set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable")
    elseif(
        (CMAKE_CXX_COMPILER_ID STREQUAL "Intel") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
    )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")
        set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -warn all -warn error")
    else()
        message(WARNING "PEDANTIC not supported for ${CMAKE_CXX_COMPILER_ID}")
        message(WARNING ${CMAKE_CXX_COMPILER_ID})
    endif()
    if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -Wno-maybe-uninitialized")
    endif()
endif()

# Define source code that goes into the SmartRedis library
list(APPEND C_CPP_SRC
    src/c/c_client.cpp
    src/c/c_configoptions.cpp
    src/c/c_dataset.cpp
    src/c/c_error.cpp
    src/c/c_logcontext.cpp
    src/c/c_logger.cpp
    src/cpp/address.cpp
    src/cpp/addressallcommand.cpp
    src/cpp/addressanycommand.cpp
    src/cpp/addressatcommand.cpp
    src/cpp/client.cpp
    src/cpp/clusterinfocommand.cpp
    src/cpp/command.cpp
    src/cpp/commandlist.cpp
    src/cpp/commandreply.cpp
    src/cpp/compoundcommand.cpp
    src/cpp/configoptions.cpp
    src/cpp/dataset.cpp
    src/cpp/dbinfocommand.cpp
    src/cpp/dbnode.cpp
    src/cpp/gettensorcommand.cpp
    src/cpp/keyedcommand.cpp
    src/cpp/logger.cpp
    src/cpp/metadata.cpp
    src/cpp/metadatafield.cpp
    src/cpp/multikeycommand.cpp
    src/cpp/nonkeyedcommand.cpp
    src/cpp/pipelinereply.cpp
    src/cpp/redis.cpp
    src/cpp/rediscluster.cpp
    src/cpp/redisserver.cpp
    src/cpp/singlekeycommand.cpp
    src/cpp/srobject.cpp
    src/cpp/stringfield.cpp
    src/cpp/tensorbase.cpp
    src/cpp/tensorpack.cpp
    src/cpp/threadpool.cpp
    src/cpp/utility.cpp
)
set(FORTRAN_SRC
    src/fortran/errors.F90
    src/fortran/client.F90
    src/fortran/configoptions.F90
    src/fortran/dataset.F90
    src/fortran/fortran_c_interop.F90
    src/fortran/logcontext.F90
    src/fortran/logger.F90
)

## Include headers from redis++ and hiredis. Included as system libraries
## to avoid the pedantic options from detecting errors in those files
include_directories(SYSTEM $<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

## Build the main SmartRedis library
set(CLIENT_SRC ${C_CPP_SRC})
add_library(smartredis ${CLIENT_SRC})
add_dependencies(smartredis hiredis redis++)
if(BUILD_SHARED_LIBS)
    set_target_properties(smartredis PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(smartredis PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (BUILD_SHARED_LIBS)
    target_link_libraries(smartredis PRIVATE libhiredis libredis++ Threads::Threads)
    add_custom_command(
        TARGET smartredis
        POST_BUILD
        # Delete hiredis artifacts
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/libhiredis.a
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/build
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/cmake
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
        # Delete redis++ artifacts
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/libredis++.a
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/share/cmake/redis++
        VERBATIM
        COMMENT "Removing artifacts from dependencies"
    )
else()

    target_link_libraries(smartredis PUBLIC
        ${CMAKE_INSTALL_PREFIX}/lib/libhiredis.a
        ${CMAKE_INSTALL_PREFIX}/lib/libredis++.a
    )
    add_custom_command(
        TARGET smartredis
        POST_BUILD
        # Delete hiredis artifacts
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/build
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/cmake
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
        # Delete redis++ artifacts
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/share/cmake/redis++
        VERBATIM
        COMMENT "Removing artifacts from dependencies"
    )
endif()

install(
    TARGETS smartredis
    EXPORT smartredis-targets
 	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install SmartRedis header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT smartredis-targets
    FILE smartredis.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/smartredis
)
include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/Config.smartredis.cmake.in
  smartredisConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/smartredis
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/smartredisConfigVersion.cmake"
  VERSION "${VERSION}"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/smartredisConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/smartredisConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/smartredis
)

# Build the libraries for the Fortran Client
if (BUILD_FORTRAN)
    add_library(smartredis-fortran ${SMARTREDIS_LINK_MODE} ${FORTRAN_SRC})
    add_dependencies(smartredis-fortran smartredis)
    target_link_libraries(smartredis-fortran PUBLIC smartredis)
    target_include_directories(smartredis PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
    if(RETAIN_RPATH)
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    endif()

    install(
        TARGETS smartredis-fortran
        EXPORT smartredis-fortran-targets
     	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT smartredis-fortran-targets
        FILE smartredis-fortran.cmake
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/smartredis-fortran
    )
    configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/Config.smartredis-fortran.cmake.in
      smartredis-fortranConfig.cmake
      INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/smartredis-fortran
    )
    write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/smartredis-fortranConfigVersion.cmake"
      VERSION "${VERSION}"
      COMPATIBILITY AnyNewerVersion
    )
    install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/smartredis-fortranConfig.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/smartredis-fortranConfigVersion.cmake
      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/smartredis-fortran
    )
else()
	message("-- Skipping Fortran client build")
endif()

# Build the Python library for SmartRedis
if(BUILD_PYTHON)
	message("-- Python client build enabled")
    FetchContent_GetProperties(pybind11)
    if(NOT pybind11_POPULATED)
        FetchContent_Populate(pybind11)
    endif()
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
	pybind11_add_module(smartredisPy
                        ${C_CPP_SRC}
	                    src/python/src/pyclient.cpp
                        src/python/src/pyconfigoptions.cpp
                        src/python/src/pydataset.cpp
                        src/python/src/pylogcontext.cpp
                        src/python/src/pysrobject.cpp
                        src/python/bindings/bind.cpp
    )
    add_dependencies(smartredisPy libhiredis libredis++)
    target_link_libraries(smartredisPy PRIVATE libhiredis libredis++)
    target_include_directories(smartredisPy PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
	install(TARGETS smartredisPy LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS smartredisPy LIBRARY DESTINATION ../src/python/module/smartredis)
else()
	message("-- Skipping Python client build")
endif()