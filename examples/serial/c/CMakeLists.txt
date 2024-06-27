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

# Project definition for the SmartRedis-Examples-Serial-C project
cmake_minimum_required(VERSION 3.13)
project(SmartRedis-Examples-Serial-C)

# Enable language support for the examples
enable_language(C)
enable_language(CXX)

# Add our custom module(s) stored in smartredis/cmake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../../cmake")
include(EnableCoverage)
if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    enable_coverage()
endif()

# Configure the build
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

# Locate dependencies
find_package(smartredis)
# Define all the examples to be built
list(APPEND EXECUTABLES
    example_put_unpack_1D
    example_put_get_3D
)

# Build the examples
foreach(EXECUTABLE ${EXECUTABLES})
    add_executable(${EXECUTABLE}_c_serial
        ${EXECUTABLE}.c
    )
    set_target_properties(${EXECUTABLE}_c_serial PROPERTIES
        OUTPUT_NAME ${EXECUTABLE}
    )
    target_link_libraries(${EXECUTABLE}_c_serial
        smartredis
    )
	install(TARGETS ${EXECUTABLE}_c_serial RUNTIME DESTINATION serial/c)
endforeach()
