#!/bin/bash -x

# get the number of processors
NPROC=$(python3 -c "import multiprocessing as mp; print(mp.cpu_count())")
echo "NPROC is $NPROC "
CMAKE=$(python3 -c "import cmake; import os; print(os.path.join(cmake.CMAKE_BIN_DIR, 'cmake'))")
echo "cmake is $CMAKE"
# Remove existing module
if [ -f ./src/python/module/smartredis/smartredisPy.*.so ]; then
    echo "Removing existing module installation"
    rm ./src/python/module/smartredis/smartredisPy.*.so
fi

if [[ -d "./build" ]]; then
    echo "Removing previous build directory"
    rm -rf ./build
fi

# make a new build directory and invoke cmake
mkdir build
cd build
ln -fs $(pwd)/../install/$MPI_ROOT/lib/* $(pwd)/../install/lib/
$CMAKE -DCMAKE_PREFIX_PATH="$(pwd)/../install/lib/" -DCMAKE_INSTALL_PREFIX="$(pwd)/../install/"  ..
make -j $NPROC
make install

if [ -f ./../install/lib/libsmartredis.so ]; then
    echo "Finished building and installing libsmartredis"
else
    echo "ERROR: libsmartredis failed to build and install"
    exit 1
fi

if [ -f ./smartredisPy.*.so ]; then
    echo "Finished building smartredisPy module"
    # move python module to module directory
    cp smartredisPy.*.so ../src/python/module/smartredis/
    cd ../
else
    echo "ERROR: smartredisPy module failed to compile"
    exit 1
fi
