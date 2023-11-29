#include "binderceptor_native.h"
#include <fcntl.h>
#include "cmn_utils.h"
#include "binderceptor_debug.h"

enum {
    EBinderceptorType_Init          = 1,
    EBinderceptorType_SetLogger     = 2,
    EBinderceptorType_SetCallback   = 3,
};

typedef struct {
    void* funcAfterFaker;
    void* funcAfterFixer;
    void* funcTarget;
    void* funcOneway;
    void* funcBefore;
} TBinderCallback;

typedef int (*funcBinderceptorCall)(int type, int argInt, void* argVal);
// ----------------------------------------------------------------------------------------------------------------
jobject BinderceptorNative::handeJavaCall(JNIEnv* env, jclass thiz, jint jtype, jint argInt, jobject jobj) {
    jobject jret = NULL;
    switch (jtype) {
    case EBinderceptorDemoType_SetLogger:
    {
        funcBinderceptorCall handle = (funcBinderceptorCall)utils_dlsym_global("libifmabinderceptor-core.so", "binderceptor_call");
        LOGD("BinderceptorNative SetLogger handle: %p", handle)
        if (!handle) break;
        int ret = handle(EBinderceptorType_SetLogger, argInt, NULL);
        LOGD("BinderceptorNative SetLogger ret: %d", ret)
    }   break;
    case EBinderceptorDemoType_Init:
    {
        funcBinderceptorCall handle = (funcBinderceptorCall)utils_dlsym_global("libifmabinderceptor-core.so", "binderceptor_call");
        LOGD("BinderceptorNative Init handle: %p", handle)
        if (!handle) break;
        int ret = handle(EBinderceptorType_Init, 0, getJvm());
        LOGD("BinderceptorNative Init ret: %d", ret)

        TBinderCallback callback = {
            (void*)BinderceptorDebug::onFrameworkBinderAfterFaker,
            (void*)BinderceptorDebug::onFrameworkBinderAfterFixer,
            (void*)BinderceptorDebug::onFrameworkBinderTarget,
            (void*)BinderceptorDebug::onFrameworkBinderOneway,
            (void*)BinderceptorDebug::onFrameworkBinderBefore,
        };
        ret = handle(EBinderceptorType_SetCallback, argInt, &callback);
        LOGD("BinderceptorNative SetLogger ret: %d", ret)
    }   break;
    default: break;
    }
    return jret;
}

