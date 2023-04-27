MAKEFLAGS += --no-print-directory
COV_FLAGS :=
SHELL:=/bin/bash
SR_DEVICE := cpu

# Params for third-party software
HIREDIS_URL := https://github.com/redis/hiredis.git
HIREDIS_VER := v1.0.0
RPP_URL := https://github.com/sewenew/redis-plus-plus.git
RPP_VER := 1.3.5
PYBIND_URL := https://github.com/pybind/pybind11.git
PYBIND_VER := v2.10.3
REDIS_URL := https://github.com/redis/redis.git
REDIS_VER := 6.0.8
REDISAI_URL := https://github.com/RedisAI/RedisAI.git
REDISAI_VER := v1.2.3
CATCH2_URL := https://github.com/catchorg/Catch2.git
CATCH2_VER := v2.13.6
LCOV_URL := https://github.com/linux-test-project/lcov.git
LCOV_VER := v1.15

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
# help: Build
# help: -------

# help: deps                           - Make SmartRedis dependencies
.PHONY: deps
deps: hiredis
deps: redis-plus-plus
deps: pybind
deps:

# help: pip-install                    - Register the SmartRedis library with pip
.PHONY: pip-install
pip-install:
	@if ! python -c "import smartredis" >& /dev/null; then pip install -e.; fi

# help: lib                            - Build SmartRedis C/C++/Python clients into a dynamic library
.PHONY: lib
lib: pip-install
lib: deps
	@bash ./build-scripts/build_lib.sh $(LIB_BUILD_ARGS)

# help: lib-with-fortran               - Build SmartRedis C/C++/Python and Fortran clients into a dynamic library
.PHONY: lib-with-fortran
lib-with-fortran: deps
	@bash ./build-scripts/build_lib.sh $(LIB_BUILD_ARGS) $(CMAKE_ARGS) -DBUILD_FORTRAN=ON

# help: test-lib                       - Build SmartRedis clients into a dynamic library with least permissive compiler settings
.PHONY: test-lib
test-lib: LIB_BUILD_ARGS="-DWERROR=ON"
test-lib: lib

# help: test-lib-with-fortran          - Build SmartRedis clients into a dynamic library with least permissive compiler settings
.PHONY: test-lib-with-fortran
test-lib-with-fortran: LIB_BUILD_ARGS="-DWERROR=ON"
test-lib-with-fortran: lib-with-fortran

# help: test-deps                      - Make SmartRedis testing dependencies
.PHONY: test-deps
test-deps: redis
test-deps: redisAI
test-deps: catch2
test-deps: lcov

# help: test-deps-gpu                  - Make SmartRedis GPU testing dependencies
.PHONY: test-deps-gpu
test-deps-gpu:
	$(error To build test-deps for GPU, please execute "make test-deps SR_DEVICE=gpu")

# help: build-tests                    - build all tests (C, C++, Fortran)
.PHONY: build-tests
build-tests: test-lib-with-fortran
	./build-scripts/build_cpp_tests.sh $(CMAKE_ARGS)
	./build-scripts/build_cpp_unit_tests.sh $(CMAKE_ARGS)
	./build-scripts/build_c_tests.sh $(CMAKE_ARGS)
	./build-scripts/build_fortran_tests.sh $(CMAKE_ARGS)


# help: build-test-cpp                 - build the C++ tests
.PHONY: build-test-cpp
build-test-cpp: test-lib
	./build-scripts/build_cpp_tests.sh
	./build-scripts/build_cpp_unit_tests.sh

# help: build-unit-test-cpp            - build the C++ unit tests
.PHONY: build-unit-test-cpp
build-unit-test-cpp: test-lib
	./build-scripts/build_cpp_unit_tests.sh

# help: build-test-c                   - build the C tests
.PHONY: build-test-c
build-test-c: test-lib
	./build-scripts/build_c_tests.sh


# help: build-test-fortran             - build the Fortran tests
.PHONY: build-test-fortran
build-test-fortran: test-lib-with-fortran
	./build-scripts/build_fortran_tests.sh


# help: build-examples                 - build all examples (serial, parallel)
.PHONY: build-examples
build-examples: lib-with-fortran
	./build-scripts/build_serial_examples.sh
	./build-scripts/build_parallel_examples.sh

# help: build-example-serial           - buld serial examples
.PHONY: build-example-serial
build-example-serial: lib
	./build-scripts/build_serial_examples.sh


# help: build-example-parallel         - build parallel examples (requires MPI)
.PHONY: build-example-parallel
build-example-parallel: lib
	./build-scripts/build_parallel_examples.sh


# help: clean-deps                     - remove third-party deps
.PHONY: clean-deps
clean-deps:
	@rm -rf ./third-party ./install/lib/libhiredis.a ./install/lib/libredis++.a


# help: clean                          - remove builds, pyc files
.PHONY: clean
clean:
	@git clean -X -f -d


# help: clobber                        - clean, remove deps, builds, (be careful)
.PHONY: clobber
clobber: clean clean-deps


# help:
# help: Style
# help: -------

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
	@pylint --rcfile=.pylintrc ./src/python/module/smartredis ./tests/python


# help:
# help: Documentation
# help: -------

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
# help: Test
# help: -------

# help: test                           - Build and run all tests (C, C++, Fortran, Python)
.PHONY: test
test: test-deps
test: build-tests
test:
	@PYTHONFAULTHANDLER=1 python -m pytest --ignore ./tests/docker -vv ./tests


# help: test-verbose                   - Build and run all tests [verbosely]
.PHONY: test-verbose
test-verbose: test-deps
test-verbose: build-tests
test-verbose:
	PYTHONFAULTHANDLER=1 python -m pytest $(COV_FLAGS) --ignore ./tests/docker -vv -s ./tests

# help: test-verbose-with-coverage                   - Build and run all tests [verbose-with-coverage]
.PHONY: test-verbose-with-coverage
test-verbose-with-coverage: test-deps
test-verbose-with-coverage: build-tests
test-verbose-with-coverage: CMAKE_ARGS="-DCOVERAGE=on"
test-verbose-with-coverage:
	PYTHONFAULTHANDLER=1 python -m pytest $(COV_FLAGS) --ignore ./tests/docker -vv -s ./tests

# help: test-c                         - Build and run all C tests
.PHONY: test-c
test-c: test-deps
test-c: build-test-c
test-c:
	@python -m pytest -vv -s ./tests/c/

# help: test-cpp                       - Build and run all C++ tests
.PHONY: test-cpp
test-cpp: test-deps
test-cpp: build-test-cpp
test-cpp: build-unit-test-cpp
test-cpp:
	@python -m pytest -vv -s ./tests/cpp/

# help: unit-test-cpp                  - Build and run unit tests for C++
.PHONY: unit-test-cpp
unit-test-cpp: test-deps
unit-test-cpp: build-unit-test-cpp
unit-test-cpp:
	@python -m pytest -vv -s ./tests/cpp/unit-tests/

# help: test-py                        - run python tests
.PHONY: test-py
test-py: test-deps
test-py:
	@PYTHONFAULTHANDLER=1 python -m pytest -vv ./tests/python/

# help: test-fortran                   - run fortran tests
.PHONY: test-fortran
test-fortran: test-deps
test-fortran: build-test-fortran
	@python -m pytest -vv ./tests/fortran/

# help: testpy-cov                     - run python tests with coverage
.PHONY: testpy-cov
testpy-cov: test-deps
testpy-cov:
	@PYTHONFAULTHANDLER=1 python -m pytest --cov=./src/python/module/smartredis/ -vv ./tests/python/

# help: test-examples                   - Build and run all examples
.PHONY: test-examples
test-examples: test-deps
test-examples: build-examples
test-examples:
	@python -m pytest -vv -s ./examples


############################################################################
# hidden build targets for third-party software

# Hiredis (hidden build target)
.phony: hiredis
hiredis: install/lib/libhiredis.a
install/lib/libhiredis.a:
	@rm -rf third-party/hiredis
	@mkdir -p third-party
	@cd third-party && \
	git clone $(HIREDIS_URL) hiredis --branch $(HIREDIS_VER) --depth=1
	@cd third-party/hiredis && \
	LIBRARY_PATH=lib CC=gcc CXX=g++ make PREFIX="../../install" static -j && \
	LIBRARY_PATH=lib CC=gcc CXX=g++ make PREFIX="../../install" install && \
	rm -f ../../install/lib/libhiredis*.so && \
	rm -f ../../install/lib/libhiredis*.dylib && \
	echo "Finished installing Hiredis"

# Redis-plus-plus (hidden build target)
.phony: redis-plus-plus
redis-plus-plus: install/lib/libredis++.a
install/lib/libredis++.a:
	@rm -rf third-party/redis-plus-plus
	@mkdir -p third-party
	@cd third-party && \
	git clone $(RPP_URL) redis-plus-plus --branch $(RPP_VER) --depth=1
	@cd third-party/redis-plus-plus && \
	mkdir -p compile && \
	cd compile && \
	cmake -DCMAKE_BUILD_TYPE=Release -DREDIS_PLUS_PLUS_BUILD_TEST=OFF -DREDIS_PLUS_PLUS_BUILD_SHARED=OFF -DCMAKE_PREFIX_PATH="../../../install/lib/" -DCMAKE_INSTALL_PREFIX="../../../install" -DCMAKE_CXX_STANDARD=17 .. && \
	CC=gcc CXX=g++ make -j && \
	CC=gcc CXX=g++ make install && \
	echo "Finished installing Redis-plus-plus"

# Pybind11 (hidden build target)
.phony: pybind
pybind: third-party/pybind/include/pybind11/pybind11.h
third-party/pybind/include/pybind11/pybind11.h:
	@mkdir -p third-party
	@cd third-party && \
	git clone $(PYBIND_URL) pybind --branch $(PYBIND_VER) --depth=1
	@mkdir -p third-party/pybind/build && \
	echo "Finished installing Pybind11"

# Redis (hidden test target)
.phony: redis
redis: third-party/redis/src/redis-server
third-party/redis/src/redis-server:
	@mkdir -p third-party
	@cd third-party && \
	git clone $(REDIS_URL) redis --branch $(REDIS_VER) --depth=1
	@cd third-party/redis && \
	CC=gcc CXX=g++ make MALLOC=libc -j && \
	echo "Finished installing redis"

# cudann-check (hidden test target)
# checks cuda dependencies for GPU build
.phony: cudann-check
cudann-check:
ifeq ($(SR_DEVICE),gpu)
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

# RedisAI (hidden test target)
.phony: redisAI
redisAI: cudann-check
redisAI: third-party/RedisAI/install-cpu/redisai.so
third-party/RedisAI/install-cpu/redisai.so:
	$(eval DEVICE_IS_GPU := $(shell test $(SR_DEVICE) == "cpu"; echo $$?))
	@mkdir -p third-party
	@cd third-party && \
	GIT_LFS_SKIP_SMUDGE=1 git clone --recursive $(REDISAI_URL) RedisAI --branch $(REDISAI_VER) --depth=1
	-@cd third-party/RedisAI && \
	CC=gcc CXX=g++ WITH_PT=1 WITH_TF=1 WITH_TFLITE=0 WITH_ORT=0 bash get_deps.sh $(SR_DEVICE) && \
	CC=gcc CXX=g++ GPU=$(DEVICE_IS_GPU) WITH_PT=1 WITH_TF=1 WITH_TFLITE=0 WITH_ORT=0 WITH_UNIT_TESTS=0 make -j -C opt clean build && \
	echo "Finished installing RedisAI"

# Catch2 (hidden test target)
.phony: catch2
catch2: third-party/catch/single_include/catch2/catch.hpp
third-party/catch/single_include/catch2/catch.hpp:
	@mkdir -p third-party
	@cd third-party && \
	git clone $(CATCH2_URL) catch --branch $(CATCH2_VER) --depth=1
	@echo "Finished installing Catch2"

# LCOV (hidden test target)
.phony: lcov
lcov: third-party/lcov/install/usr/local/bin/lcov
third-party/lcov/install/usr/local/bin/lcov:
	@mkdir -p third-party
	@cd third-party && \
	git clone $(LCOV_URL) lcov --branch $(LCOV_VER) --depth=1
	@cd third-party/lcov && \
	mkdir -p install && \
	CC=gcc CXX=g++ DESTDIR="install/" make install && \
	echo "Finished installing LCOV"
