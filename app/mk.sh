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
# pre
# ----------------------------------------------------------------
if [ -d "$OUT_PATH/libs" ] ; then
  mkdir -p ./libs/arm64-v8a
  cp -f $OUT_PATH/libs/arm64-v8a/*.so ./libs/arm64-v8a
  mkdir -p ./libs/armeabi-v7a
  cp -f $OUT_PATH/libs/armeabi-v7a/*.so ./libs/armeabi-v7a
fi

# ----------------------------------------------------------------
# build
# ----------------------------------------------------------------
cd ..
if [ $BUILD_TYPE = debug ] ; then
    ./gradlew :$MODULE_NAME:assembleDebug
else
    ./gradlew :$MODULE_NAME:assembleRelease
fi
cd -

# ----------------------------------------------------------------
# build end
# ----------------------------------------------------------------
cp -f $LOCAL_PATH/build/outputs/apk/$BUILD_TYPE/*.apk $OUT_PATH/$MODULE_NAME.apk
