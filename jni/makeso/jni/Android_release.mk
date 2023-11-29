LOCAL_PATH := $(call my-dir)
SHARED_PATH := ../../../out/shared
STATIC_PATH := ../../../out/jni/release

ifeq ($(TARGET_ARCH),arm)
    LOCAL_SHARE_LIB_PATH := $(SHARED_PATH)/armeabi-v7a
    LOCAL_STATIC_LIB_PATH := $(STATIC_PATH)/armeabi-v7a
else
    ifeq ($(TARGET_ARCH),arm64)
        LOCAL_SHARE_LIB_PATH := $(SHARED_PATH)/arm64-v8a
        LOCAL_STATIC_LIB_PATH := $(STATIC_PATH)/arm64-v8a
    else
        ifeq ($(TARGET_ARCH),x86)
            LOCAL_SHARE_LIB_PATH := $(SHARED_PATH)/x86
            LOCAL_STATIC_LIB_PATH := $(STATIC_PATH)/x86
        else
            ifeq ($(TARGET_ARCH),x86_64)
                LOCAL_SHARE_LIB_PATH := $(SHARED_PATH)/x86_64
                LOCAL_STATIC_LIB_PATH := $(STATIC_PATH)/x86_64
            else
                echo "unknown platform"
            endif
        endif
    endif
endif

######################################### prebuilt static libraries
include $(CLEAR_VARS)
LOCAL_MODULE        := ifmabinderceptor-native
LOCAL_C_INCLUDES    += $(VSA_INCLUDE_PATH)
LOCAL_SRC_FILES     := $(LOCAL_STATIC_LIB_PATH)/libifmabinderceptor-native.a
include $(PREBUILT_STATIC_LIBRARY)

########################### build for .so
include $(CLEAR_VARS)
LOCAL_MODULE        := libifmabinderceptor-jni
LOCAL_C_INCLUDES    += $(VSA_INCLUDE_PATH)
LOCAL_CFLAGS        := -Os -fvisibility=hidden -Wall -Werror
LOCAL_LDLIBS        += -llog -L$(LOCAL_SHARE_LIB_PATH) -lstdc++
ifneq ($(TARGET_ARCH),arm64)
LOCAL_CFLAGS        += -DHAVE_PTHREADS
endif
LOCAL_WHOLE_STATIC_LIBRARIES := ifmabinderceptor-native
include $(BUILD_SHARED_LIBRARY)
