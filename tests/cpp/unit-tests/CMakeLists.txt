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

project(CppClientUnitTester)

cmake_minimum_required(VERSION 3.13)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS --coverage)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_library(REDISPP redis++ PATHS ../../../install/lib NO_DEFAULT_PATH REQUIRED)
find_library(HIREDIS hiredis PATHS ../../../install/lib NO_DEFAULT_PATH REQUIRED)
find_package(Threads REQUIRED)
set(EXT_LIBRARIES ${REDISPP} ${HIREDIS})

include_directories(SYSTEM
    /usr/local/include
    ../../../include
	../../../install/include
)

# The source files to assess coverage on

# The unit tests to be executed
#set(UNIT_TESTS
#	main.cpp
#	test_aggregation_list.cpp
#	test_client.cpp
#	test_client_ensemble.cpp
#	test_client_prefixing.cpp
#	test_dataset.cpp
#	test_tensorbase.cpp
#	test_dbnode.cpp
#	test_commandlist.cpp
#	test_tensor.cpp
#	test_metadata.cpp
#	test_stringfield.cpp
#	test_commandreply.cpp
#	test_singlekeycommand.cpp
#	test_addressatcommand.cpp
#	test_compoundcommand.cpp
#	test_multikeycommand.cpp
#	test_addressanycommand.cpp
#	test_dbinfocommand.cpp
#	test_clusterinfocommand.cpp
#	test_redisserver.cpp
#	test_logger.cpp
#	test_ssdb.cpp
#)

file(GLOB UNIT_TESTS CONFIGURE_DEPENDS ./*.cpp)

add_executable(cpp_unit_tests ${SOURCES} ${UNIT_TESTS})

target_link_libraries(cpp_unit_tests PUBLIC ${EXT_LIBRARIES} PRIVATE Threads::Threads)
