#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <android/log.h>
#include "binder_def.h"
#include "binder_linux.h"
#include "binder_parcel.h"
#include "cmn_utils.h"
#include "cmn_env.h"
#include "binder_jvm_parcel.h"
#include "binder_android_parcel.h"

#ifndef INT32_MAX
#define INT32_MAX ((int32_t)(2147483647))
#endif

static void BinderParcel_freeIgnore(AndroidParcel* parcel, const uint8_t* data, size_t dataSize) {
    // do nothing, just read parcel
}

// -------------------------------------------------------------------------------------------------------------------------
BinderParcel::BinderParcel()
: mJvmParcel(NULL), mParcel(NULL)
{
    mParcel = new AndroidParcel();
    mJvmParcel = new JvmParcel(mParcel);
}

BinderParcel::BinderParcel(const uint8_t* data, size_t dataSize)
: mJvmParcel(NULL), mParcel(NULL)
{
    mParcel = new AndroidParcel();
    mJvmParcel = new JvmParcel(mParcel);
    __ASSERT(data && 0 < dataSize)
    if (__LIKELY(data && 0 < dataSize)) {
        setDataForReader(data, dataSize);
    }
}

BinderParcel::~BinderParcel() {
    MEM_DELETE(mParcel)
    MEM_DELETE(mJvmParcel)
}

void BinderParcel::setDataForReader(const uint8_t* data, size_t dataSize) {
    mParcel->ipcSetDataForReader(data, dataSize, (AndroidParcel::release_func) BinderParcel_freeIgnore);
}

void BinderParcel::setDataForWriter(size_t desired) {
    mParcel->ipcSetDataForWriter(desired);
}

const uint8_t* BinderParcel::data() const {
    return mParcel->data();
}

size_t BinderParcel::dataSize() const {
    return mParcel->dataSize();
}

size_t BinderParcel::dataAvail() const {
    return mParcel->dataAvail();
}

size_t BinderParcel::dataPosition() const {
    return mParcel->dataPosition();
}

status_t BinderParcel::setDataCapacity(size_t size) {
    return mParcel->setDataCapacity(size);
}

void BinderParcel::setDataPosition(size_t pos) const {
    mParcel->setDataPosition(pos);
}

status_t BinderParcel::appendFrom(const BinderParcel *parcel, size_t start, size_t len) {
    return mParcel->appendFrom(parcel->mParcel, start, len);
}

status_t BinderParcel::errorCheck() const {
    return mParcel->errorCheck();
}

status_t BinderParcel::write(const void* data, size_t len) {
    return mParcel->write(data, len);
}

void* BinderParcel::writeInplace(size_t len) {
    return mParcel->writeInplace(len);
}

status_t BinderParcel::writeInt32(int32_t val) {
    return mParcel->writeInt32(val);
}

status_t BinderParcel::writeDouble(double val) {
    return mParcel->writeDouble(val);
}

status_t BinderParcel::writeInt64(int64_t val) {
    return mParcel->writeInt64(val);
}

status_t BinderParcel::skip(size_t len) const {
    if (__LIKELY(readInplace(len))) return NO_ERROR;
    return NOT_ENOUGH_DATA;
}

status_t BinderParcel::skipCString8() const {
    size_t len = 0;
    const uint8_t* data = readBytes(len);
    if (__LIKELY(data)) {
        return NO_ERROR;
    }
    return NOT_ENOUGH_DATA;
}

status_t BinderParcel::skipString16() const {
    int32_t size = readInt32();
    if (-1 == size) return NO_ERROR;
    if (0 <= size && size < INT32_MAX) {
        if (readInplace((size+1)*sizeof(char16_t))) {
            return NO_ERROR;
        }
    }
    return NOT_ENOUGH_DATA;
}

status_t BinderParcel::skipToken() const {
    int32_t size = 0;
    if (NO_ERROR != readInt32(&size)) return NOT_ENOUGH_DATA;// skip strict mode
    if (ENV_SDK_INT_Q_10_29 <= CmnEnv::sdkVersion() && NO_ERROR != readInt32(&size)) return NOT_ENOUGH_DATA;// skip strict flags
    if (ENV_SDK_INT_R_11_30 <= CmnEnv::sdkVersion() && NO_ERROR != readInt32(&size)) return NOT_ENOUGH_DATA;// skip strict flags
    return skipString16();// skip token desc
}

status_t BinderParcel::skipBinderObject() const {
    return skip(__STRONG_BINDER_SIZE__);
}

status_t BinderParcel::read(void* outData, size_t len) const {
    return mParcel->read(outData, len);
}

const void* BinderParcel::readInplace(size_t len) const {
    return mParcel->readInplace(len);
}

int32_t BinderParcel::readInt32() const {
    return mParcel->readInt32();
}

status_t BinderParcel::readInt32(int32_t *pArg) const {
    return mParcel->readInt32(pArg);
}

double BinderParcel::readDouble() const {
    return mParcel->readDouble();
}

status_t BinderParcel::readDouble(double *pArg) const {
    return mParcel->readDouble(pArg);
}

int64_t BinderParcel::readInt64() const {
    return mParcel->readInt64();
}

status_t BinderParcel::readInt64(int64_t *pArg) const {
    return mParcel->readInt64(pArg);
}

const char16_t* BinderParcel::readString16Inplace(size_t* outLen) const {
    return mParcel->readString16Inplace(outLen);
}

const uint8_t* BinderParcel::readBytes(size_t & len/*out*/) const {
    len = readInt32();
    // watch for potential int overflow
    if (len <= dataAvail()) {
        return (const uint8_t*)readInplace(len);
    }
    return NULL;
}

char* BinderParcel::readCStringEx() const {
    char* str = NULL;
    size_t len = 0;
    const uint8_t* data = readBytes(len);
    if (__LIKELY(data)) {
        str = (char*) malloc(len + 1);
        if (__LIKELY(str)) {
            memcpy(str, data, len);
            str[len] = '\0';
        }
    }
    return str;
}

char* BinderParcel::readString16To8() const {
    size_t len = 0;
    const char16_t* path16 = readString16Inplace(&len);
    if (path16 && len < __1_M_SIZE_VALUE__) {
        return utils_convert_char16((const uint16_t *) path16, len);
    }
    return NULL;
}

status_t BinderParcel::writeBytes(const uint8_t* val, size_t len) {
    if (!val) {
        return writeInt32(-1);
    }
    status_t err = writeInt32(len);
    if (NO_ERROR == err) {
        err = write(val, len);
    }
    return err;
}

status_t BinderParcel::writeString16Ex(const char16_t* str, size_t len)
{
    if (str == NULL) return writeInt32(-1);

    status_t err = writeInt32(len);
    if (err == NO_ERROR) {
        len *= sizeof(char16_t);
        uint8_t* data = (uint8_t*)writeInplace(len+sizeof(char16_t));
        if (data) {
            memcpy(data, str, len);
            *reinterpret_cast<char16_t*>(data+len) = 0;
            return NO_ERROR;
        }
        err = errorCheck();
    }
    return err;
}

status_t BinderParcel::writeStringBy16(const char* str, size_t len8)
{
    if (str == NULL) return writeInt32(-1);

    status_t err = writeInt32(len8);
    if (err == NO_ERROR) {
        size_t len = len8 * sizeof(char16_t) + sizeof(char16_t);
        char16_t* data16 = (char16_t*)writeInplace(len);
        if (data16) {
            for (int i = 0; i < len8; i++) {
                data16[i] = str[i];
            }
            data16[len8] = 0;
            return NO_ERROR;
        }
        err = errorCheck();
    }
    return err;
}

status_t BinderParcel::writeCStringEx(const char* str) {
    if (__LIKELY(str)) {
        return writeBytes((const uint8_t*)str, strlen(str));
    } else {
        return writeInt32(-1);
    }
}

void BinderParcel::freeData() {
    mParcel->freeData();
}

int32_t BinderParcel::getInt32(int32_t def) const {
    if (__LIKELY(sizeof(int32_t) <= dataAvail())) {
        return *(reinterpret_cast<const int32_t*>(data()+dataPosition()));
    }
    return def;
}

int32_t BinderParcel::valueOfInt32(const void* data, int32_t def) {
    if (__LIKELY(data)) {
        def = *(reinterpret_cast<const int32_t*>(data));
    }
    return def;
}

bool BinderParcel::init(JNIEnv* env) {
    return JvmParcel::init(env);
}

jobject BinderParcel::obtain(JNIEnv* env) {
    return mJvmParcel->obtain(env);
}

void BinderParcel::recycle(JNIEnv* env, bool copyData) {
    if (copyData) {
        mJvmParcel->marshall(env);
    }
    mJvmParcel->recycle(env);
}
