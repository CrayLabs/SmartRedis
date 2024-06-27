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

# General settings
MAKEFLAGS += --no-print-directory
SHELL:=/bin/bash
CWD := $(shell pwd)

# Build variables
NPROC := $(shell nproc 2>/dev/null || python -c "import multiprocessing as mp; print (mp.cpu_count())" 2>/dev/null || echo 4)
INSTALL_PREFIX := $(CWD)/install
TEST_PREFIX := $(CWD)/tests
EXAMPLES_PREFIX := $(CWD)/examples/bin
BUILD_FORTRAN := OFF
BUILD_PYTHON := OFF
BUILD_TYPE := Release
LINK_TYPE := shared
PEDANTIC := OFF

# Test dependencies
REDIS_URL := https://github.com/redis/redis.git
REDIS_VER := 7.2.4
REDISAI_URL :=  https://github.com/RedisAI/RedisAI.git
CATCH2_URL := https://github.com/catchorg/Catch2.git
CATCH2_VER := v2.13.6
LCOV_URL := https://github.com/linux-test-project/lcov.git
LCOV_VER := v2.0

# Test variables
DEP_CC := gcc
DEP_CXX := g++
COV_FLAGS :=
SR_TEST_REDIS_MODE := Clustered
SR_TEST_UDS_FILE := /tmp/redis.sock
SR_TEST_PORT := 6379
SR_TEST_NODES := 3
SR_TEST_REDISAI_VER := v1.2.7
SR_TEST_DEVICE := cpu
SR_TEST_PYTEST_FLAGS := -vv -s
ifeq ($(LINK_TYPE), shared)
	BUILD_SHARED_LIBS=on
else
	BUILD_SHARED_LIBS=off
endif
SR_TEST_INSTALL_PREFIX = $(CWD)/install/$(BUILD_TYPE)/$(LINK_TYPE)
# Do not remove this block. It is used by the 'help' rule when
# constructing the help output.
# help:
# help: SmartRedis Makefile help
# help:

# help: help                           - display this makefile's help information
.PHONY: help
help:
	@grep "^# help\:" Makefile | grep -v grep | sed 's/\# help\: //' | sed 's/\# help\://'

# help:
# help: Build variables
# help: ---------------
# help:
# help: These variables affect the way that the SmartRedis library is built. Each
# help: has several options; the first listed is the default. Use by appending
# help: the variable name and setting after the make target, e.g.
# help:    make lib BUILD_TYPE=Debug Static BUILD_FORTRAN=ON
# help:
# help: BUILD_TYPE {Release, Debug, Coverage} -- optimization level for the build
# help: LINK_TYPE {shared, static} -- type of linking for the smartredis libraries
# help: PEDANTIC {OFF, ON} -- GNU only; enable pickiest compiler settings,
# help: 					     currently fails due to warnings on newer GNU versions
# help: BUILD_FORTRAN {OFF, ON} -- Enable/disable build of Fortran library
# help: BUILD_PYTHON {OFF, ON} -- Enable/disable build of Python library
# help:
# help: Test variables
# help: --------------
# help:
# help: These variables affect the way that the SmartRedis library is tested. Each
# help: has several options; the first listed is the default. Use by appending
# help: the variable name and setting after the make target, e.g.
# help:    make test BUILD_TYPE=Debug BUILD_SHARED_LIBS=off BUILD_FORTRAN=ON
# help:
# help: SR_TEST_REDIS_MODE {Clustered, Standalone} -- type of Redis backend launched for tests
# help: SR_TEST_PORT (Default: 6379) -- first port for Redis server(s)
# help: SR_TEST_NODES (Default: 3) Number of shards to intantiate for a clustered Redis database
# help: SR_TEST_REDISAI_VER {v1.2.7} -- version of RedisAI to use for tests
# help: SR_TEST_DEVICE {cpu, gpu} -- device type to test on. Warning, this variable is CASE SENSITIVE!
# help: SR_TEST_PYTEST_FLAGS (default: "-vv -s"): Verbosity flags to use with pytest

# help:
# help: Build targets
# help: -------------

# help: lib                            - Build SmartRedis C/C++/Python clients into a dynamic library
.PHONY: lib
lib:
lib:
	@cmake -S . -B build/$(BUILD_TYPE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) \
		-DPEDANTIC=$(PEDANTIC) -DBUILD_FORTRAN=$(BUILD_FORTRAN) -DBUILD_PYTHON=$(BUILD_PYTHON) \
		-DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX)
	@cmake --build build/$(BUILD_TYPE) -- -j $(NPROC)
	@cmake --install build/$(BUILD_TYPE)

# help: lib-with-fortran               - Build SmartRedis C/C++/Python and Fortran clients into a dynamic library
.PHONY: lib-with-fortran
lib-with-fortran: BUILD_FORTRAN=ON
lib-with-fortran: lib

# help: test-lib                       - Build SmartRedis clients into a dynamic library with least permissive compiler settings
.PHONY: test-lib
test-lib: PEDANTIC=OFF # Re-enable after warnings fixed
test-lib: lib

# help: test-lib-with-fortran          - Build SmartRedis clients into a dynamic library with least permissive compiler settings
.PHONY: test-lib-with-fortran
test-lib-with-fortran: PEDANTIC=off
test-lib-with-fortran: lib-with-fortran

# help: test-deps                      - Make SmartRedis testing dependencies
.PHONY: test-deps
test-deps: redis
test-deps: redisAI
test-deps: lcov

# help: test-deps-gpu                  - Make SmartRedis GPU testing dependencies
.PHONY: test-deps-gpu
test-deps-gpu: SR_TEST_DEVICE=gpu
test-deps-gpu: test-deps

# help: build-tests                    - build all tests (C, C++, Fortran)
.PHONY: build-tests
build-tests: test-lib-with-fortran
build-tests: build-unit-test-cpp
build-tests: build-test-cpp
build-tests: build-test-c
build-tests: build-test-fortran


# help: build-test-cpp                 - build the C++ tests
.PHONY: build-test-cpp
build-test-cpp: test-lib
build-test-cpp: catch2
	@cmake -S tests/cpp -B build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/cpp \
		-DBUILD_TYPE=$(BUILD_TYPE) -DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) \
		-Dsmartredis_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis \
		-DCMAKE_INSTALL_PREFIX=$(TEST_PREFIX)/cpp
	@cmake --build build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/cpp -- -j $(NPROC)
	@cmake --install build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/cpp

# help: build-unit-test-cpp            - build the C++ unit tests
.PHONY: build-unit-test-cpp
build-unit-test-cpp: test-lib
build-unit-test-cpp: catch2
	@cmake -S tests/cpp/unit-tests -B build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/cpp/unit-tests \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) \
		-DCMAKE_INSTALL_PREFIX=$(TEST_PREFIX)/cpp \
		-Dsmartredis_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis
	@cmake --build build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/cpp/unit-tests -- -j $(NPROC)
	@cmake --install build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/cpp/unit-tests

# help: build-test-c                   - build the C tests
.PHONY: build-test-c
build-test-c: test-lib
	@cmake -S tests/c -B build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/c \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) \
		-Dsmartredis_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis \
		-DCMAKE_INSTALL_PREFIX=$(TEST_PREFIX)/c
	@cmake --build build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/c -- -j $(NPROC)
	@cmake --install build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/c


# help: build-test-fortran             - build the Fortran tests
.PHONY: build-test-fortran
build-test-fortran: BUILD_FORTRAN=ON
build-test-fortran: test-lib-with-fortran
	@cmake -S tests/fortran -B build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/fortran \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) \
		-DCMAKE_INSTALL_PREFIX=$(TEST_PREFIX)/fortran \
		-Dsmartredis_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis \
		-Dsmartredis-fortran_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis-fortran
	@cmake --build build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/fortran -- -j $(NPROC)
	@cmake --install build/$(BUILD_TYPE)/tests/$(LINK_TYPE)/fortran


# help: build-examples                 - build all examples (serial, parallel)
.PHONY: build-examples
build-examples: lib
	@cmake -S examples -B build/$(BUILD_TYPE)/examples/$(LINK_TYPE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) -DBUILD_FORTRAN=$(BUILD_FORTRAN) \
		-DCMAKE_INSTALL_PREFIX=$(EXAMPLES_PREFIX) \
		-Dsmartredis_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis \
		-Dsmartredis-fortran_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis-fortran
	@cmake --build build/$(BUILD_TYPE)/examples/$(LINK_TYPE) -- -j $(NPROC)
	@cmake --install build/$(BUILD_TYPE)/examples/$(LINK_TYPE)/


# help: build-example-serial           - buld serial examples
.PHONY: build-example-serial
build-example-serial: lib
	@cmake -S examples/serial -B build/$(BUILD_TYPE)/examples/$(LINK_TYPE)/serial \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) -DBUILD_FORTRAN=$(BUILD_FORTRAN) \
		-DCMAKE_INSTALL_PREFIX=$(EXAMPLES_PREFIX) \
		-Dsmartredis_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis \
		-Dsmartredis-fortran_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis-fortran
	@cmake --build build/$(BUILD_TYPE)/examples/$(LINK_TYPE)/serial


# help: build-example-parallel         - build parallel examples (requires MPI)
.PHONY: build-example-parallel
build-example-parallel: lib
	@cmake -S examples/parallel -B build/$(BUILD_TYPE)/examples/$(LINK_TYPE)/parallel \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_SHARED_LIBS=$(BUILD_SHARED_LIBS) -DBUILD_FORTRAN=$(BUILD_FORTRAN) \
		-DCMAKE_INSTALL_PREFIX=$(EXAMPLES_PREFIX) \
		-Dsmartredis_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis \
		-Dsmartredis-fortran_DIR=$(INSTALL_PREFIX)/share/cmake/smartredis-fortran
	@cmake --build build/$(BUILD_TYPE)/examples/$(LINK_TYPE)/parallel


# help: clean-deps                     - remove third-party deps
.PHONY: clean-deps
clean-deps:
	@rm -rf ./third-party ./install/lib/libhiredis.a ./install/lib/libredis++.a


# help: clean                          - remove builds, pyc files
.PHONY: clean
clean:
	@git clean -X -f -d
	rm -rf build


# help: clobber                        - clean, remove deps, builds, (be careful)
.PHONY: clobber
clobber: clean clean-deps


# help:
# help: Style targets
# help: -------------

# help: style                          - Sort imports and format with black
.PHONY: style
style: sort-imports format


# help: check-style                    - check code style compliance
.PHONY: check-style
check-style: check-sort-imports check-format


# help: format                         - perform code style format
.PHONY: format
format:
	@black ./src/python/module/smartredis ./tests/python/


# help: check-format                   - check code format compliance
.PHONY: check-format
check-format:
	@black --check ./src/python/module/smartredis ./tests/python/


# help: sort-imports                   - apply import sort ordering
.PHONY: sort-imports
sort-imports:
	@isort ./src/python/module/smartredis ./tests/python/ --profile black


# help: check-sort-imports             - check imports are sorted
.PHONY: check-sort-imports
check-sort-imports:
	@isort  ./src/python/module/smartredis ./tests/python/ --check-only --profile black


# help: check-lint                     - run static analysis checks
.PHONY: check-lint
check-lint:
	@pylint --rcfile=.pylintrc ./src/python/module/smartredis


# help:
# help: Documentation targets
# help: ---------------------

# help: docs                           - generate project documentation
.PHONY: docs
docs:
	@cd doc; make html

# help: cov                            - generate html coverage report for Python client
.PHONY: cov
cov:
	@coverage html
	@echo if data was present, coverage report is in htmlcov/index.html

# help:
# help: Test targets
# help: ------------

# Build Pytest flags to skip various subsets of the tests
ifeq ($(BUILD_PYTHON),OFF)
SKIP_PYTHON = --ignore ./tests/python
endif
ifeq ($(BUILD_FORTRAN),OFF)
SKIP_FORTRAN = --ignore ./tests/fortran
endif
SKIP_DOCKER := --ignore ./tests/docker

# Build SSDB string for clustered database
SSDB_STRING := 127.0.0.1:$(SR_TEST_PORT)
PORT_RANGE := $(shell seq `expr $(SR_TEST_PORT) + 1` 1 `expr $(SR_TEST_PORT) + $(SR_TEST_NODES) - 1`)
SSDB_STRING += $(foreach P,$(PORT_RANGE),",127.0.0.1:$(P)")
SSDB_STRING := $(shell echo $(SSDB_STRING) | tr -d " ")

# Run test cases with a freshly instantiated standalone Redis server
# Parameters:
# 	1: the test directory in which to run tests
define run_smartredis_tests_with_standalone_server
	echo "Launching standalone Redis server" && \
	export SR_TEST_DEVICE=$(SR_TEST_DEVICE) SR_DB_TYPE=Standalone && \
	export SSDB=127.0.0.1:$(SR_TEST_PORT) && \
	python utils/launch_redis.py --port $(SR_TEST_PORT) --nodes 1 \
		--rai $(SR_TEST_REDISAI_VER) --device $(SR_TEST_DEVICE) && \
	echo "Running standalone tests" && \
	PYTHONFAULTHANDLER=1 python -m pytest $(SR_TEST_PYTEST_FLAGS) $(COV_FLAGS) \
		$(SKIP_DOCKER) $(SKIP_PYTHON) $(SKIP_FORTRAN) \
		--bin-path $(2) --build-fortran $(BUILD_FORTRAN) $(1)  ; \
	(testresult=$$?; \
	echo "Shutting down standalone Redis server" && \
	python utils/launch_redis.py --port $(SR_TEST_PORT) --nodes 1 --stop && \
	test $$testresult -eq 0 || echo "Standalone tests failed"; exit $$testresult) && \
	echo "Standalone tests complete"
endef

# Run test cases with a freshly instantiated clustered Redis server
# Parameters:
# 	1: the test directory in which to run tests
define run_smartredis_tests_with_clustered_server
	echo "Launching clustered Redis server" && \
	export SR_TEST_DEVICE=$(SR_TEST_DEVICE) SR_DB_TYPE=Clustered && \
	export SSDB=$(SSDB_STRING) && \
	python utils/launch_redis.py --port $(SR_TEST_PORT) --nodes $(SR_TEST_NODES) \
		--rai $(SR_TEST_REDISAI_VER) --device $(SR_TEST_DEVICE) && \
	echo "Running clustered tests" && \
	PYTHONFAULTHANDLER=1 python -s -m pytest $(SR_TEST_PYTEST_FLAGS) $(COV_FLAGS) \
		$(SKIP_DOCKER) $(SKIP_PYTHON) $(SKIP_FORTRAN) \
		--bin-path $(2) --build-fortran $(BUILD_FORTRAN) $(1)  ; \
	(testresult=$$?; \
	echo "Shutting down clustered Redis server" && \
	python utils/launch_redis.py --port $(SR_TEST_PORT) \
		--nodes $(SR_TEST_NODES) --stop; \
	test $$testresult -eq 0 || echo "Clustered tests failed"; exit $$testresult) && \
	echo "Clustered tests complete"
endef

# Run test cases with a freshly instantiated standalone Redis server
# connected via a Unix Domain Socket
# Parameters:
# 	1: the test directory in which to run tests
define run_smartredis_tests_with_uds_server
	echo "Launching standalone Redis server with Unix Domain Socket support"
	export SR_TEST_DEVICE=$(SR_TEST_DEVICE) SR_DB_TYPE=Standalone && \
	export SSDB=unix://$(SR_TEST_UDS_FILE) && \
	python utils/launch_redis.py --port $(SR_TEST_PORT) --nodes 1 \
		--rai $(SR_TEST_REDISAI_VER) --device $(SR_TEST_DEVICE) \
		--udsport $(SR_TEST_UDS_FILE) && \
	echo "Running standalone tests with Unix Domain Socket connection" && \
	PYTHONFAULTHANDLER=1 python -m pytest $(SR_TEST_PYTEST_FLAGS) $(COV_FLAGS) \
		$(SKIP_DOCKER) $(SKIP_PYTHON) $(SKIP_FORTRAN) \
		--bin-path $(2)  --build-fortran $(BUILD_FORTRAN) $(1)  ; \
	(testresult=$$?; \
	echo "Shutting down standalone Redis server with Unix Domain Socket support" && \
	python utils/launch_redis.py --port $(SR_TEST_PORT) --nodes 1 \
		--udsport $(SR_TEST_UDS_FILE) --stop; \
	test $$testresult -eq 0 || echo "UDS tests failed"; exit $$testresult) && \
	echo "UDS tests complete"
endef

# Run test cases with freshly instantiated Redis servers
# Parameters:
# 	1: the test directory in which to run tests
define run_smartredis_tests_with_server
	$(if $(or $(filter $(SR_TEST_REDIS_MODE),Standalone),
	          $(filter $(SR_TEST_REDIS_MODE),All)),
		$(call run_smartredis_tests_with_standalone_server,$(1),$(2))
	)
	$(if $(or $(filter $(SR_TEST_REDIS_MODE),Clustered),
	          $(filter $(SR_TEST_REDIS_MODE),All)),
		$(call run_smartredis_tests_with_clustered_server,$(1),$(2))
	)
	$(if $(or $(filter $(SR_TEST_REDIS_MODE),UDS),
	          $(filter $(SR_TEST_REDIS_MODE),All)),
		$(if $(filter-out $(shell uname -s),Darwin),
			$(call run_smartredis_tests_with_uds_server,$(1),$(2)),
			@echo "Skipping: Unix Domain Socket is not supported on MacOS"
		)
	)
endef

# help: test                           - Build and run all tests (C, C++, Fortran, Python)
.PHONY: test
test: test-deps
test: build-tests
test: SR_TEST_PYTEST_FLAGS := -vv
test:
	@$(call run_smartredis_tests_with_server,./tests, $(TEST_PREFIX))

# help: test-verbose                   - Build and run all tests [verbosely]
.PHONY: test-verbose
test-verbose: test-deps
test-verbose: build-tests
test-verbose: SR_TEST_PYTEST_FLAGS := -vv -s
test-verbose:
	@$(call run_smartredis_tests_with_server,./tests, $(TEST_PREFIX))

# help: test-verbose-with-coverage     - Build and run all tests [verbose-with-coverage]
.PHONY: test-verbose-with-coverage
test-verbose-with-coverage: BUILD_TYPE := Coverage
test-verbose-with-coverage: test-deps
test-verbose-with-coverage: build-tests
test-verbose-with-coverage: SR_TEST_PYTEST_FLAGS := -vv -s
test-verbose-with-coverage:
	@$(call run_smartredis_tests_with_server,./tests, $(TEST_PREFIX))

# help: test-c                         - Build and run all C tests
.PHONY: test-c
test-c: build-test-c
test-c: SR_TEST_PYTEST_FLAGS := -vv -s
test-c:
	@$(call run_smartredis_tests_with_server,./tests/c, $(TEST_PREFIX))

# help: test-cpp                       - Build and run all C++ tests
.PHONY: test-cpp
test-cpp: build-test-cpp
test-cpp: build-unit-test-cpp
test-cpp: SR_TEST_PYTEST_FLAGS := -vv -s
test-cpp:
	@$(call run_smartredis_tests_with_server,./tests/cpp, $(TEST_PREFIX))

# help: unit-test-cpp                  - Build and run unit tests for C++
.PHONY: unit-test-cpp
unit-test-cpp: build-unit-test-cpp
unit-test-cpp: SR_TEST_PYTEST_FLAGS := -vv -s
unit-test-cpp:
	@$(call run_smartredis_tests_with_server,./tests/cpp/unit-tests, $(TEST_PREFIX))

# help: test-py                        - run python tests
.PHONY: test-py
test-py: test-deps
test-py: BUILD_PYTHON := ON
test-py: lib
test-py: SR_TEST_PYTEST_FLAGS := -vv
test-py:
	@$(call run_smartredis_tests_with_server,./tests/python, $(TEST_PREFIX))

# help: test-fortran                   - run fortran tests
.PHONY: test-fortran
test-fortran: BUILD_FORTRAN := ON
test-fortran: build-test-fortran
test-fortran: SR_TEST_PYTEST_FLAGS := -vv -s
test-fortran:
	@$(call run_smartredis_tests_with_server,./tests/fortran, $(TEST_PREFIX))

# help: testpy-cov                     - run python tests with coverage
.PHONY: testpy-cov
testpy-cov: test-deps
testpy-cov: BUILD_PYTHON := ON
testpy-cov: SR_TEST_PYTEST_FLAGS := -vv
testpy-cov: COV_FLAGS := --cov=./src/python/module/smartredis/
testpy-cov:
	@$(call run_smartredis_tests_with_server,./tests/python, $(TEST_PREFIX))

# help: test-examples                   - Build and run all examples
.PHONY: test-examples
test-examples: test-deps
test-examples: build-examples
testpy-cov: SR_TEST_PYTEST_FLAGS := -vv -s
test-examples:
	@$(call run_smartredis_tests_with_server,./examples,$(EXAMPLES_PREFIX))


############################################################################
# hidden build targets for third-party software

# cudann-check (hidden test target)
# checks cuda dependencies for GPU build
.PHONY: cudann-check
cudann-check:
ifeq ($(SR_TEST_DEVICE),gpu)
ifndef CUDA_HOME
	$(error ERROR: CUDA_HOME is not set)
endif
ifndef CUDNN_INCLUDE_DIR
	$(error ERROR: CUDNN_INCLUDE_DIR is not set)
endif
ifeq (,$(wildcard $(CUDNN_INCLUDE_DIR)/cudnn.h))
	$(error ERROR: could not find cudnn.h at $(CUDNN_INCLUDE_DIR))
endif
ifndef CUDNN_LIBRARY
	$(error ERROR: CUDNN_LIBRARY is not set)
endif
ifeq (,$(wildcard $(CUDNN_LIBRARY)/libcudnn.so))
	$(error ERROR: could not find libcudnn.so at $(CUDNN_LIBRARY))
endif
endif

# Redis (hidden test target)
.PHONY: redis
third-party/redis:
	@mkdir -p third-party
	@cd third-party && \
	git clone $(REDIS_URL) redis --branch $(REDIS_VER) --depth=1

redis: third-party/redis/src/redis-server
third-party/redis/src/redis-server: third-party/redis
	@cd third-party/redis && \
	make CC=$(DEP_CC) CXX=$(DEP_CXX) MALLOC=libc -j $(NPROC) && \
	echo "Finished installing redis"

# RedisAI (hidden test target)
third-party/RedisAI:
	@mkdir -p third-party
	@cd third-party && \
	rm -rf RedisAI/$(SR_TEST_REDISAI_VER) && \
	GIT_LFS_SKIP_SMUDGE=1 git clone --recursive $(REDISAI_URL) RedisAI/$(SR_TEST_REDISAI_VER) \
		--branch $(SR_TEST_REDISAI_VER) --depth=1

.PHONY: redisAI
redisAI: cudann-check
redisAI: third-party/RedisAI/$(SR_TEST_REDISAI_VER)/install-$(SR_TEST_DEVICE)/redisai.so
third-party/RedisAI/$(SR_TEST_REDISAI_VER)/install-$(SR_TEST_DEVICE)/redisai.so: third-party/RedisAI
	@echo in third-party/RedisAI/$(SR_TEST_REDISAI_VER)/install-$(SR_TEST_DEVICE)/redisai.so:
	$(eval DEVICE_IS_GPU := $(shell test $(SR_TEST_DEVICE) == "cpu"; echo $$?))
	@cd third-party/RedisAI/$(SR_TEST_REDISAI_VER) && \
		WITH_PT=1 WITH_TF=1 WITH_TFLITE=0 WITH_ORT=0 bash get_deps.sh \
		$(SR_TEST_DEVICE)
	@cd third-party/RedisAI/$(SR_TEST_REDISAI_VER) && \
		GPU=$(DEVICE_IS_GPU) WITH_PT=1 WITH_TF=1 WITH_TFLITE=0 WITH_ORT=0 \
		WITH_PT=1 WITH_TF=1 WITH_TFLITE=0 WITH_ORT=0 bash get_deps.sh \
		WITH_UNIT_TESTS=0 make CC=$(DEP_CC) CXX=$(DEP_CXX) -j $(NPROC) -C opt clean
	@cd third-party/RedisAI/$(SR_TEST_REDISAI_VER) && \
		GPU=$(DEVICE_IS_GPU) WITH_PT=1 WITH_TF=1 WITH_TFLITE=0 WITH_ORT=0 \
		WITH_UNIT_TESTS=0 make CC=$(DEP_CC) CXX=$(DEP_CXX) -C opt && \
	echo "Finished installing RedisAI"

# Catch2 (hidden test target)
.PHONY: catch2
catch2: third-party/catch/single_include/catch2/catch.hpp
third-party/catch/single_include/catch2/catch.hpp:
	@mkdir -p third-party
	@cd third-party && \
	git clone $(CATCH2_URL) catch --branch $(CATCH2_VER) --depth=1
	@echo "Finished installing Catch2"

# LCOV (hidden test target)
.PHONY: lcov
lcov: third-party/lcov/install/bin/lcov
third-party/lcov/install/bin/lcov:
	@echo Installing LCOV
	@mkdir -p third-party
	@cd third-party && \
	git clone $(LCOV_URL) lcov --branch $(LCOV_VER) --depth=1
	@cd third-party/lcov && \
	mkdir -p install && \
	make CC=$(DEP_CC) CXX=$(DEP_CXX) PREFIX=$(CWD)/third-party/lcov/install/ install && \
	echo "Finished installing LCOV"


