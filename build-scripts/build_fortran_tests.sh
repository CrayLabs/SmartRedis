#!/bin/bash

CMAKE=$(which cmake)

# Any command line arguments are assumed to be CMake arguments
CMAKE_ARGS=$@

cd ./tests/fortran/

# setup build dirs
if [[ -d "./build" ]]; then
    echo "Removing previous cpp unit test build directory"
    rm -rf ./build
fi
mkdir build
cd ./build

DO_FORTRAN="yes"

if [ "$(uname)" == "Darwin" ]; then
    DO_FORTRAN="yes"
fi

if [[ $DO_FORTRAN == "yes" ]]; then
    # TODO add platform dependent build step here
    $CMAKE .. $CMAKE_ARGS

    if [ $? != 0 ]; then
        echo "ERROR: cmake for Fortran tests failed"
        cd ..
        exit 1
    fi

    make

    if [ $? != 0 ]; then
        echo "ERROR: failed to make Fortran tests"
        cd ..
        exit 1
    fi

    cd ../
    echo "Fortran tests built"
else
    echo "Skipping Fortran test build"
fi

