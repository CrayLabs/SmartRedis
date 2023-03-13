#!/bin/bash

# Any command line arguments are assumed to be CMake arguments
CMAKE_ARGS=$@

CMAKE=$(which cmake)

cd ./tests/c/

# setup build dirs
if [[ -d "./build" ]]; then
    echo "Removing previous cpp unit test build directory"
    rm -rf ./build
fi
mkdir build
cd ./build

# TODO add platform dependent build step here
$CMAKE .. $CMAKE_ARGS

if [ $? != 0 ]; then
    echo "ERROR: cmake for C tests failed"
    cd ..
    exit 1
fi

make -j

if [ $? != 0 ]; then
    echo "ERROR: failed to make C tests"
    cd ..
    exit 1
fi

cd ../

echo