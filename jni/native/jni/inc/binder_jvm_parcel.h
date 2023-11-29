#ifndef __BINDER_JVM_PARCEL_H__
#define __BINDER_JVM_PARCEL_H__

#include <jni.h>
#include "cmn_macro.h"

class AndroidParcel;
class JvmParcel {
public:
    static bool init(JNIEnv* env);

    JvmParcel(AndroidParcel* pcl);

    jobject obtain(JNIEnv* env);
    void recycle(JNIEnv* env);
    void marshall(JNIEnv* env);

protected:
    inline bool isInvalid(JNIEnv* env) const { return !env || !mParcelObj || !sParcelClass; }

    size_t dataSize(JNIEnv* env);
    void unmarshall(JNIEnv* env, const void* data, size_t dataSize);

private:
    AndroidParcel* mParcel;
    jobject mParcelObj;
    static jclass sParcelClass;
};

#endif // __BINDER_JVM_PARCEL_H__
