#!/usr/bin/env bash

set -x

echo "Starting Build Entry"
echo "HOME:" $HOME
echo "GITHUB_WORKSPACE:" $GITHUB_WORKSPACE
echo "GITHUB_EVENT_PATH:" $GITHUB_EVENT_PATH
echo "PWD:" `pwd`

cd ${GITHUB_WORKSPACE}
cd $PEGASUS_BUILD_TYPE
make -j$(nproc --all) pegasus_regress
REGRESS_PEGASUS=$?
if [ ${REGRESS_PEGASUS} -ne 0 ]; then
    echo "ERROR: regress of Pegasus FAILED!!!"
    exit 1
fi
