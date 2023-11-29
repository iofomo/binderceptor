#include <sys/stat.h>
#include "binderceptor_native.h"

static JavaVM* g_jvm = NULL;
// ------------------------------------------------------------------------------------------------------------------------------------------------------
JNIEnv* jni_get_env(JavaVM *jvm, bool& attach) {
    if (!jvm) {
        return NULL;
    }
    g_jvm = jvm;

    JNIEnv* env = NULL;
    int state = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (state == JNI_EDETACHED) {
        if (JNI_OK == g_jvm->AttachCurrentThread(&env, NULL)) {
            attach = true;
        } else {
            env = NULL;
        }
    } else if (state == JNI_EVERSION) {
        env = NULL;
    }
    return env;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    bool attach = false;
    JNIEnv *env = jni_get_env(vm, attach);
    if (NULL == env) {
        return -3;
    }
    jclass clazz = env->FindClass("android/app/ifma/mts/binderceptor/BinderceptorManager");
    if (NULL == clazz) {
        return -4;
    }

    const JNINativeMethod methods[] = {
        {"callNative", "(IILjava/lang/Object;)Ljava/lang/Object;", (void *) BinderceptorNative::handeJavaCall},
    };
    int ret = env->RegisterNatives(clazz, methods, NELEM(methods));
    if (ret < 0) {
        return ret;
    }

    return JNI_VERSION_1_6;
}

JavaVM* BinderceptorNative::getJvm() {
    return g_jvm;
}
