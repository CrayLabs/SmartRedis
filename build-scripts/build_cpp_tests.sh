#!/bin/bash

# Any command line arguments are assumed to be CMake arguments
CMAKE_ARGS=$@

CMAKE=$(which cmake)

cd ./tests/cpp/

# setup build dirs
if [[ -d "./build" ]]; then
    echo "Removing previous cpp test build directory"
    rm -rf ./build
fi
mkdir build
cd ./build

# TODO add platform dependent build step here
$CMAKE .. $CMAKE_ARGS

if [ $? != 0 ]; then
    echo "ERROR: cmake for CPP tests failed"
    cd ..
    exit 1
fi

make -j

if [ $? != 0 ]; then
    echo "ERROR: failed to make CPP tests"
    cd ..
    exit 1
fi

cd ../

echo
