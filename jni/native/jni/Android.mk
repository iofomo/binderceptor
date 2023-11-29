LOCAL_PATH := $(call my-dir)

MODULE_SRC_FILES := \
		src/cmn_env.cpp \
		src/cmn_utils.cpp \
		src/binder_info.cpp \
        src/binderceptor_main.cpp \
        src/binderceptor_native.cpp \
        src/binderceptor_debug.cpp \


########################### build for shared library
include $(CLEAR_VARS)
LOCAL_MODULE        := libifmabinderceptor-native
LOCAL_C_INCLUDES    += $(LOCAL_PATH)/inc
LOCAL_CFLAGS        += -fvisibility=hidden
LOCAL_SRC_FILES     += $(MODULE_SRC_FILES)
ifneq ($(TARGET_ARCH),arm64)
LOCAL_CFLAGS        += -DHAVE_PTHREADS
endif
include $(BUILD_STATIC_LIBRARY)
