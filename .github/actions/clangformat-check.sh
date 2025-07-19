#!/usr/bin/env bash

set -x

source "/usr/share/miniconda/etc/profile.d/conda.sh"
conda activate riscv_func_model

echo "Starting Build Entry"
echo "HOME:" $HOME
echo "GITHUB_WORKSPACE:" $GITHUB_WORKSPACE
echo "GITHUB_EVENT_PATH:" $GITHUB_EVENT_PATH
echo "CONDA_PREFIX:" $CONDA_PREFIX
echo "PWD:" `pwd`

CXX_COMPILER=${COMPILER/clang/clang++}

#
# Compile Sparta Infra (always build with release)
#   Have other build types point to release
#
echo "Building Sparta Infra"
cd ${GITHUB_WORKSPACE}/map/sparta
mkdir -p release
cd release
CC=$COMPILER CXX=$CXX_COMPILER cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX}
if [ $? -ne 0 ]; then
    echo "ERROR: CMake for Sparta framework failed"
    exit 0
fi
make -j$(nproc --all) install > install.log
BUILD_SPARTA=$?
if [ ${BUILD_SPARTA} -ne 0 ]; then
    echo "ERROR: build sparta FAILED!!!"
    echo "$(<install.log)"
    exit 1
fi
rm install.log

cd ${GITHUB_WORKSPACE}
mkdir $PEGASUS_BUILD_TYPE
cd $PEGASUS_BUILD_TYPE
CC=$COMPILER CXX=$CXX_COMPILER cmake .. -DCMAKE_BUILD_TYPE=$PEGASUS_BUILD_TYPE
if [ $? -ne 0 ]; then
    echo "ERROR: CMake for pegasus failed"
    exit 1
fi
make clangformat-check
BUILD_PEGASUS=$?
if [ ${BUILD_PEGASUS} -ne 0 ]; then
    echo "ERROR: clangformat check of Pegasus FAILED!!!"
    exit 1
fi
