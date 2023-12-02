#include <stdlib.h>
#include <binder_jvm_parcel.h>
#include <binder_android_parcel.h>

jclass JvmParcel::sParcelClass = NULL;
// ---------------------------------------------------------------------------------------------------------------------
bool JvmParcel::init(JNIEnv* env) {
    jclass jcls = env->FindClass("android/os/Parcel");
    __ASSERT(jcls)
    sParcelClass = static_cast<jclass>(env->NewGlobalRef(jcls));
    return NULL != sParcelClass;
}

JvmParcel::JvmParcel(AndroidParcel* pcl)
: mParcel(pcl), mParcelObj(NULL) {
    __ASSERT(mParcel)
}

jobject JvmParcel::obtain(JNIEnv* env) {
    if (__UNLIKELY(!env)) return NULL;

    jmethodID method = env->GetStaticMethodID(sParcelClass, "obtain", "()Landroid/os/Parcel;");
    __ASSERT(method)
    if (__UNLIKELY(!method)) return NULL;

    mParcelObj = env->CallStaticObjectMethod(sParcelClass, method);
    __ASSERT(mParcelObj)
    if (__UNLIKELY(!mParcelObj)) return NULL;

    if (__LIKELY(0 < mParcel->dataSize())) {
        method = env->GetMethodID(sParcelClass, "setDataPosition", "(I)V");
        __ASSERT(method)
        if (__LIKELY(method)) {
            unmarshall(env, mParcel->data(), mParcel->dataSize());
            env->CallVoidMethod(mParcelObj, method, mParcel->dataPosition());
        }
    }

#ifdef __ENABLE_LOG_VALUE__
    method = env->GetMethodID(sParcelClass, "dataPosition", "()I");
    __ASSERT(method)
    int pos = env->CallIntMethod(mParcelObj, method);
    __ASSERT(mParcel->dataPosition() == pos)
    method = env->GetMethodID(sParcelClass, "dataSize", "()I");
    __ASSERT(method)
    int size = env->CallIntMethod(mParcelObj, method);
    __ASSERT(mParcel->dataSize() == size)
#endif

    return mParcelObj;
}

void JvmParcel::recycle(JNIEnv* env) {
    if (__UNLIKELY(isInvalid(env))) return ;

    jmethodID method = env->GetMethodID(sParcelClass, "recycle", "()V");
    if (__LIKELY(method)) {
        env->CallVoidMethod(mParcelObj, method);
    }
    if (__LIKELY(mParcelObj)) {
        env->DeleteLocalRef(mParcelObj);
    }
    mParcelObj = NULL;
}

static void* jni_jbytearray_new(JNIEnv* env, jbyteArray jbyteArr, int& dataSize) {
    if (__UNLIKELY(!env || !jbyteArr)) return NULL;

    jbyte* data = NULL;
    dataSize = env->GetArrayLength(jbyteArr);
    if (__LIKELY(0 < dataSize)) {
        data = (jbyte *) malloc(dataSize);
        if (__LIKELY(data)) {
            env->GetByteArrayRegion(jbyteArr, 0, dataSize, data);
        }
    }

    return data;
}

void JvmParcel::marshall(JNIEnv* env) {
    if (__UNLIKELY(isInvalid(env))) return ;
    if (__UNLIKELY(dataSize(env) <= mParcel->dataSize())) return ;// has no new data

    jmethodID method = env->GetMethodID(sParcelClass, "marshall", "()[B");
    __ASSERT(method)
    if (__UNLIKELY(!method)) return ;

    jbyteArray array = (jbyteArray) env->CallObjectMethod(mParcelObj, method);
    __ASSERT(array)
    if (__UNLIKELY(!array)) return ;

    int dataSize = 0;
    void* data = jni_jbytearray_new(env, array, dataSize);
    if (__LIKELY(data)) {
        mParcel->setData((const uint8_t*)data, dataSize);
        mParcel->setDataPosition(0);
        free(data);
    }
    env->DeleteLocalRef(array);
}

size_t JvmParcel::dataSize(JNIEnv* env) {
    jmethodID method = env->GetMethodID(sParcelClass, "dataSize", "()I");
    __ASSERT(method)
    if (__LIKELY(method)) {
        return env->CallIntMethod(mParcelObj, method);
    }
    return 0;
}

void JvmParcel::unmarshall(JNIEnv* env, const void* _data, size_t _dataSize) {
    if (__UNLIKELY(isInvalid(env))) return ;

    jbyteArray array = env->NewByteArray(_dataSize);
    __ASSERT(array)
    if (__UNLIKELY(!array)) return ;

    env->SetByteArrayRegion(array, 0, _dataSize, reinterpret_cast<const jbyte*>(_data));

    jmethodID method = env->GetMethodID(sParcelClass, "unmarshall", "([BII)V");
    __ASSERT(method)
    if (__LIKELY(method)) {
        env->CallVoidMethod(mParcelObj, method, array, 0, _dataSize);
    }
    env->DeleteLocalRef(array);
}
