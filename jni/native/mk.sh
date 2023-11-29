#!/bin/bash
set -u
set -e

LOCAL_PATH=`pwd`
OUT_PATH=$1
OUT_JNI_PATH=$2
BUILD_TYPE=$3
MODULE_NAME=$4

# ------------------------------------------------------------------------------------------------------
ndk-build -C jni clean
#ndk-build NDK_DEBUG=0
ndk-build -C jni

# ------------------------------------------------------------------------------------------------------
cp obj/local/armeabi-v7a/*.a $OUT_JNI_PATH/armeabi-v7a
cp obj/local/arm64-v8a/*.a $OUT_JNI_PATH/arm64-v8a
#cp obj/local/x86/*.a $OUT_JNI_PATH/x86
#cp obj/local/x86_64/*.a $OUT_JNI_PATH/x86_64

# ------------------------------------------------------------------------------------------------------
rm -rf obj
