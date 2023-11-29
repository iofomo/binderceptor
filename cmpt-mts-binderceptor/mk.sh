#!/usr/bin/env bash
#
# @ date: 2020.07.27 17:19
#

set -u
set -e

LOCAL_PATH=`pwd`
OUT_PATH=$1
BUILD_TYPE=$2
MODULE_NAME=$3

# ----------------------------------------------------------------
# build
# ----------------------------------------------------------------
cd ..
if [ $BUILD_TYPE = debug ] ; then
    ./gradlew $MODULE_NAME:testDebug
    ./gradlew :$MODULE_NAME:assembleDebug
else
    ./gradlew $MODULE_NAME:testRelease
    ./gradlew :$MODULE_NAME:assembleRelease
fi
cd -

# ----------------------------------------------------------------
# build end
# ----------------------------------------------------------------
cp -f $LOCAL_PATH/build/outputs/aar/*-$BUILD_TYPE.aar $OUT_PATH/$MODULE_NAME.aar
