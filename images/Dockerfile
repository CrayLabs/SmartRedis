#
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
#

## Builder image
FROM ubuntu:22.04 as builder

# Install tooling
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -q -y --no-install-recommends \
    ca-certificates git \
    make cmake gcc g++ libc6-dev python3 libpython3-dev pip gfortran python-is-python3 && \
    rm -rf /var/lib/apt/lists/*

# Get source code from current clone
COPY . /usr/local/src/SmartRedis

# To build a docker image from another repository,
# users should uncomment the following WORKDIR and
# RUN commands and remove the COPY command above.
#WORKDIR /usr/local/src/
#RUN git config --global advice.detachedHead false && \
#    git clone https://github.com/CrayLabs/SmartRedis.git --branch develop

# Compile and install
WORKDIR /usr/local/src/SmartRedis
RUN pip3 install --upgrade pip
RUN make clobber && make lib BUILD_FORTRAN=ON BUILD_PYTHON=ON INSTALL_PREFIX=/usr/local
RUN pip install . && rm -rf ~/.cache/pip \
    && rm -rf build tests examples images utils third-party doc
