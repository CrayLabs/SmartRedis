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

# Select CMake build type based on SR_BUILD variable
if(SR_BUILD == Release)
    set(CMAKE_BUILD_TYPE RELEASE)
    set(SRLIB_NAME_SUFFIX "")
elseif(SR_BUILD == Debug)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(SRLIB_NAME_SUFFIX "-debug")
elseif(SR_BUILD == Coverage)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(SRLIB_NAME_SUFFIX "-coverage")
    set(COVERAGE ON)
else()
    message(ERROR "Unrecognized build type specified in SR_BUILD")
endif()

# Select CMake linkage based of SR_LINK variable
if(SR_LINK == Static)
    set(CMAKE_LINK_LIBRARY_SUFFIX .a)
elseif(SR_LINK == Shared)
    set(CMAKE_LINK_LIBRARY_SUFFIX .so)
else()
    message(ERROR "Unrecognized link type specified in SR_LINK")
endif()

# Identify the SmartRedis library name based on the build and link
set(SMARTREDIS_LIB smartredis${SRLIB_NAME_SUFFIX})
