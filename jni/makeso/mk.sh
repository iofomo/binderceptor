#!/bin/bash
set -u
set -e

LOCAL_PATH=`pwd`
OUT_PATH=$1
OUT_JNI_PATH=$2
BUILD_TYPE=$3
#MODULE_NAME=$4

# ------------------------------------------------------------------------------------------------------
ndk-build -C jni NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application_${BUILD_TYPE}.mk clean
ndk-build -C jni NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application_${BUILD_TYPE}.mk

# ------------------------------------------------------------------------------------------------------
OUT_PATH=../../out/$BUILD_TYPE

mkdir -p ${OUT_PATH}/libs/arm64-v8a
cp -f jni/libs/arm64-v8a/*.so ${OUT_PATH}/libs/arm64-v8a
mkdir -p ${OUT_PATH}/libs/armeabi-v7a
cp -f jni/libs/armeabi-v7a/*.so ${OUT_PATH}/libs/armeabi-v7a

mkdir -p ${OUT_PATH}/libs-obj/arm64-v8a
cp -f jni/obj/local/arm64-v8a/*.so ${OUT_PATH}/libs-obj/arm64-v8a
mkdir -p ${OUT_PATH}/libs-obj/armeabi-v7a
cp -f jni/obj/local/armeabi-v7a/*.so ${OUT_PATH}/libs-obj/armeabi-v7a
# ------------------------------------------------------------------------------------------------------
rm -rf jni/obj
rm -rf jni/libs
