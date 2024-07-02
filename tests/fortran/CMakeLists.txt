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

# Project definition for the SmartRedis-Tests-Fortran project
cmake_minimum_required(VERSION 3.13)
project(SmartRedis-Tests-Fortran)

# Enable language support for the tests
enable_language(Fortran)

# Add our custom module(s) stored in smartredis/cmake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../cmake")
include(EnableCoverage)
if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    enable_coverage()
endif()

# Configure the build
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)
find_package(smartredis REQUIRED)
find_package(smartredis-fortran REQUIRED)

# Stuff the test_utils into a library to enable parallel builds
add_library(test-utils STATIC test_utils.F90)

# Define all the tests to be built
list(APPEND EXECUTABLES
	 client_test_dataset_aggregation
	 client_test_dataset
	 client_test_ensemble
	 client_test_initialized
	 client_test_misc_tensor
	 client_test_mnist_multigpu
	 client_test_mnist
	 client_test_prefixing
	 client_test_put_get_1D
	 client_test_put_get_2D
	 client_test_put_get_3D
	 client_test_put_get_unpack_dataset
	 client_test_logging
	 client_test_errors
	 client_test_configoptions
)

# Build the tests
foreach(EXECUTABLE ${EXECUTABLES})
    add_executable(${EXECUTABLE}_fortran_test
        ${EXECUTABLE}.F90
    )
    set_target_properties(${EXECUTABLE}_fortran_test PROPERTIES
        OUTPUT_NAME ${EXECUTABLE}
		LINKER_LANGUAGE Fortran
    )
    target_link_libraries(${EXECUTABLE}_fortran_test
        smartredis smartredis-fortran test-utils
    )
    target_include_directories(${EXECUTABLE}_fortran_test PRIVATE ${smartredis_INCLUDE_DIR} ${smartredis-fortran_INCLUDE_DIR})
    install(TARGETS ${EXECUTABLE}_fortran_test)
endforeach()