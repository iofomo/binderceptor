#ifndef __BINDERCEPTOR_NATIVE_H__
#define __BINDERCEPTOR_NATIVE_H__

#include <jni.h>
#include "cmn_macro.h"

enum {
    EBinderceptorDemoType_Init              = 1,
    EBinderceptorDemoType_SetLogger         = 2,
};

enum {
    EBinderceptorDemoFlag_Print_Simple           = 0x1,
    EBinderceptorDemoFlag_Print_Transaction_Data = 0x2,
    EBinderceptorDemoFlag_Print_Write_Read       = 0x4,
    EBinderceptorDemoFlag_Filter                 = 0x8,
};

class BinderceptorNative {
public:
    static jobject handeJavaCall(JNIEnv* env, jclass thiz, jint jtype, jint argInt, jobject jobj);

private:
    static JavaVM* getJvm();
};

#endif// end of __BINDERCEPTOR_NATIVE_H__
