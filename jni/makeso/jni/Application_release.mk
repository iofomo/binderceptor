NDK_MODULE_PATH := $(call my-dir)
APP_OPTIM := release
APP_ABI := armeabi-v7a arm64-v8a
APP_PLATFORM := android-21
APP_MODULES := libifmabinderceptor-jni
APP_STL := c++_static
APP_CPPFLAGS := -std=c++11
#APP_CPPFLAGS += -fpermissive
APP_BUILD_SCRIPT := Android_release.mk
