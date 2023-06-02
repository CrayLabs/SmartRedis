# BSD 2-Clause License
#
# Copyright (c) 2021-2023, Hewlett Packard Enterprise
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

# Project definition for the SmartRedis-Examples-Parallel-Fortran project
cmake_minimum_required(VERSION 3.13)
project(SmartRedis-Examples-Parallel-Fortran)

# Enable language support for the examples
enable_language(Fortran)

# Configure the build
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../..")
include(smartredis_defs)

# Assume by default that users should link against the install directory in this repository
if(NOT DEFINED SMARTREDIS_INSTALL_PATH)
  set(SMARTREDIS_INSTALL_PATH "../../../install/")
endif()

# Locate dependencies
find_package(MPI REQUIRED)
IF(NOT MPI_Fortran_FOUND)
	message(FATAL_ERROR "Could not find Fortran MPI components")
endif()
find_library(SR_LIB ${SMARTREDIS_LIB}
    PATHS ${SMARTREDIS_INSTALL_PATH}/lib NO_DEFAULT_PATH
    REQUIRED
    ${SMARTREDIS_LINK_MODE}
)
find_library(SR_FTN_LIB ${SMARTREDIS_FORTRAN_LIB}
	PATHS ${SMARTREDIS_INSTALL_PATH}/lib NO_DEFAULT_PATH
	REQUIRED
    ${SMARTREDIS_LINK_MODE}
)
set(SMARTREDIS_LIBRARIES
	${SR_LIB}
	${SR_FTN_LIB}
)

# Define include directories for header files
include_directories(SYSTEM
    /usr/local/include
    ${MPI_INCLUDE_PATH}
    ${SMARTREDIS_INSTALL_PATH}/include
)

# Stuff the example_utils into a library to enable parallel builds
add_library(example_utils STATIC example_utils.F90)

# Define all the examples to be built
list(APPEND EXECUTABLES
	 smartredis_dataset
	 smartredis_mnist
	 smartredis_put_get_3D
)

# Build the examples
foreach(EXECUTABLE ${EXECUTABLES})
	add_executable(${EXECUTABLE}_fortran_parallel
		${EXECUTABLE}.F90
	)
	set_target_properties(${EXECUTABLE}_fortran_parallel PROPERTIES
		OUTPUT_NAME ${EXECUTABLE}
	)
	target_link_libraries(${EXECUTABLE}_fortran_parallel
		${SMARTREDIS_LIBRARIES}
		MPI::MPI_Fortran
		example_utils
	)
endforeach()
