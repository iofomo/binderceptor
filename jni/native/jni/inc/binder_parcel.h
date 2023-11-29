/*
 * from: \frameworks\native\include\binder\Parcel.h
 *  only for parcel parse
 */

#ifndef __BINDER_PARCEL_H__
#define __BINDER_PARCEL_H__

#include "cmn_macro.h"
#include <jni.h>

typedef int32_t     status_t;

// ---------------------------------------------------------------------------
class JvmParcel;
class AndroidParcel;

class BinderParcel {
public:
    BinderParcel();
    BinderParcel(const uint8_t* data, size_t dataSize);// for read
    virtual ~BinderParcel();

    static bool init(JNIEnv* env);

    void setDataForReader(const uint8_t* data, size_t dataSize);
    void setDataForWriter(size_t desired);

    const uint8_t*      data() const;
    size_t              dataSize() const;
    size_t              dataAvail() const;
    size_t              dataPosition() const;
    status_t            setDataCapacity(size_t size);
    void                setDataPosition(size_t pos) const;
    status_t            appendFrom(const BinderParcel *parcel, size_t start, size_t len);
    status_t            errorCheck() const;

    status_t            write(const void* data, size_t len);
    void*               writeInplace(size_t len);
    status_t            writeInt32(int32_t val);
    status_t            writeDouble(double val);
    status_t            writeInt64(int64_t val);
    // extends write methods
    status_t            writeBytes(const uint8_t* val, size_t len);
    status_t            writeString16Ex(const char16_t* str, size_t len);
    status_t            writeStringBy16(const char* str, size_t len);
    status_t            writeCStringEx(const char* str);

    status_t            read(void* outData, size_t len) const;
    const void*         readInplace(size_t len) const;
    int32_t             readInt32() const;
    status_t            readInt32(int32_t *pArg) const;
    double              readDouble() const;
    status_t            readDouble(double *pArg) const;
    int64_t             readInt64() const;
    status_t            readInt64(int64_t *pArg) const;
    const char16_t*     readString16Inplace(size_t* outLen) const;
    // extends read methods
    const uint8_t*      readBytes(size_t & len/*out*/) const;
    char*               readCStringEx() const;
    char*               readString16To8() const;

    // extends skip methods
    status_t skipToken() const;
    status_t skipCString8() const;
    status_t skipString16() const;
    status_t skip(size_t len) const;
    status_t skipBinderObject() const;

    void freeData();

    // read but not move position
    int32_t getInt32(int32_t def) const;
    // public static methods
    static int32_t valueOfInt32(const void* data, int32_t def);

    jobject obtain(JNIEnv* env);
    void recycle(JNIEnv* env, bool copyData);
    inline AndroidParcel* androidParcel() { return mParcel; }

private:
    AndroidParcel* mParcel;
    JvmParcel* mJvmParcel;
};

#endif // __BINDER_PARCEL_H__
