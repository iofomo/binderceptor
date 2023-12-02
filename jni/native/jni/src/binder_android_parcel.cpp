#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <cmn_macro.h>
#include "binder_android_parcel.h"

#ifndef INT32_MAX
#define INT32_MAX ((int32_t)(2147483647))
#endif
#ifndef SIZE_T_MAX
#define SIZE_T_MAX 0xffffffffUL
#endif

#define EX_HAS_REPLY_HEADER -128
// ---------------------------------------------------------------------------
#define PAD_SIZE_UNSAFE(s) (((s)+3)&~3)

static size_t pad_size(size_t s) {
    if (s > (SIZE_T_MAX - 3)) {
        abort();
    }
    return PAD_SIZE_UNSAFE(s);
}

AndroidParcel::AndroidParcel()
{
    initState();
}

AndroidParcel::~AndroidParcel()
{
    freeDataNoInit();
}

const uint8_t* AndroidParcel::data() const
{
    return mData;
}

size_t AndroidParcel::dataSize() const
{
    return (mDataSize > mDataPos ? mDataSize : mDataPos);
}

size_t AndroidParcel::dataAvail() const
{
    size_t result = dataSize() - dataPosition();
    if (result > INT32_MAX) {
        abort();
    }
    return result;
}

size_t AndroidParcel::dataPosition() const
{
    return mDataPos;
}

size_t AndroidParcel::dataCapacity() const
{
    return mDataCapacity;
}

status_t AndroidParcel::setDataSize(size_t size)
{
    if (size > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    status_t err;
    err = continueWrite(size);
    if (err == NO_ERROR) {
        mDataSize = size;
    }
    return err;
}

void AndroidParcel::setDataPosition(size_t pos) const
{
    if (pos > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        abort();
    }

    mDataPos = pos;
}

status_t AndroidParcel::setDataCapacity(size_t size)
{
    if (size > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    if (size > mDataCapacity) return continueWrite(size);
    return NO_ERROR;
}

status_t AndroidParcel::setData(const uint8_t* buffer, size_t len)
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    status_t err = restartWrite(len);
    if (err == NO_ERROR) {
        memcpy(const_cast<uint8_t*>(data()), buffer, len);
        mDataSize = len;
    }
    return err;
}

status_t AndroidParcel::appendFrom(uint8_t *_data, size_t offset, int len)
{
    if (len == 0) {
        return NO_ERROR;
    }

    // range checks against the source AndroidParcel size
    if ((offset > len)
        || (offset + len > len)) {
        return BAD_VALUE;
    }

    // grow data
    status_t err = growData(len);
    if (err != NO_ERROR) {
        return err;
    }

    // append data
    memcpy(mData + mDataPos, _data + offset, len);
    mDataPos += len;
    mDataSize += len;

    return NO_ERROR;
}
status_t AndroidParcel::appendFrom(const AndroidParcel *parcel, size_t offset, size_t len)
{
    status_t err;
    const uint8_t *data = parcel->mData;
    if (len == 0) {
        return NO_ERROR;
    }

    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    // range checks against the source parcel size
    if ((offset > parcel->mDataSize)
            || (len > parcel->mDataSize)
            || (offset + len > parcel->mDataSize)) {
        return BAD_VALUE;
    }

    if ((mDataSize+len) > mDataCapacity) {
        // grow data
        err = growData(len);
        if (err != NO_ERROR) {
            return err;
        }
    }

    // append data
    memcpy(mData + mDataPos, data + offset, len);
    mDataPos += len;
    mDataSize += len;

    err = NO_ERROR;

    return err;
}

status_t AndroidParcel::errorCheck() const
{
    return mError;
}

void AndroidParcel::setError(status_t err)
{
    mError = err;
}

status_t AndroidParcel::finishWrite(size_t len)
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    //printf("Finish write of %d\n", len);
    mDataPos += len;

    if (mDataPos > mDataSize) {
        mDataSize = mDataPos;
    }
    //printf("New pos=%d, size=%d\n", mDataPos, mDataSize);
    return NO_ERROR;
}

status_t AndroidParcel::writeUnpadded(const void* data, size_t len)
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    size_t end = mDataPos + len;
    if (end < mDataPos) {
        // integer overflow
        return BAD_VALUE;
    }

    if (end <= mDataCapacity) {
restart_write:
        memcpy(mData+mDataPos, data, len);
        return finishWrite(len);
    }

    status_t err = growData(len);
    if (err == NO_ERROR) goto restart_write;
    return err;
}

status_t AndroidParcel::write(const void* data, size_t len)
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    void* const d = writeInplace(len);
    if (d) {
        memcpy(d, data, len);
        return NO_ERROR;
    }
    return mError;
}

void* AndroidParcel::writeInplace(size_t len)
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return NULL;
    }

    const size_t padded = pad_size(len);

    // sanity check for integer overflow
    if (mDataPos+padded < mDataPos) {
        return NULL;
    }

    if ((mDataPos+padded) <= mDataCapacity) {
restart_write:
        //printf("Writing %ld bytes, padded to %ld\n", len, padded);
        uint8_t* const data = mData+mDataPos;

        // Need to pad at end?
        if (padded != len) {
#if BYTE_ORDER == BIG_ENDIAN
            static const uint32_t mask[4] = {
                0x00000000, 0xffffff00, 0xffff0000, 0xff000000
            };
#elif BYTE_ORDER == LITTLE_ENDIAN
            static const uint32_t mask[4] = {
                0x00000000, 0x00ffffff, 0x0000ffff, 0x000000ff
            };
#endif
            //printf("Applying pad mask: %p to %p\n", (void*)mask[padded-len],
            //    *reinterpret_cast<void**>(data+padded-4));
            *reinterpret_cast<uint32_t*>(data+padded-4) &= mask[padded-len];
        }

        finishWrite(padded);
        return data;
    }

    status_t err = growData(padded);
    if (err == NO_ERROR) goto restart_write;
    return NULL;
}
status_t AndroidParcel::writeInt32(int32_t val)
{
    return writeAligned(val);
}

status_t AndroidParcel::writeInt64(int64_t val)
{
    return writeAligned(val);
}

status_t AndroidParcel::writeFloat(float val)
{
    return writeAligned(val);
}



status_t AndroidParcel::writeDouble(double val)
{
    return writeAligned(val);
}


status_t AndroidParcel::writeCString(const char* str)
{
    return write(str, strlen(str)+1);
}

status_t AndroidParcel::writeNoException()
{
//    binder::Status status;
    return writeInt32(0);
}

void AndroidParcel::remove(size_t /*start*/, size_t /*amt*/)
{
//    LOG_ALWAYS_FATAL("AndroidParcel::remove() not yet implemented!");
}

status_t AndroidParcel::read(void* outData, size_t len) const
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    if ((mDataPos+pad_size(len)) >= mDataPos && (mDataPos+pad_size(len)) <= mDataSize
            && len <= pad_size(len)) {
        memcpy(outData, mData+mDataPos, len);
        mDataPos += pad_size(len);
        return NO_ERROR;
    }
    return NOT_ENOUGH_DATA;
}

const void* AndroidParcel::readInplace(size_t len) const
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return NULL;
    }

    if ((mDataPos+pad_size(len)) >= mDataPos && (mDataPos+pad_size(len)) <= mDataSize
            && len <= pad_size(len)) {
        const void* data = mData+mDataPos;
        mDataPos += pad_size(len);
//        ALOGV("readInplace Setting data pos of %p to %zu", this, mDataPos);
        return data;
    }
    return NULL;
}

template<class T>
status_t AndroidParcel::readAligned(T *pArg) const {
//    COMPILE_TIME_ASSERT_FUNCTION_SCOPE(PAD_SIZE_UNSAFE(sizeof(T)) == sizeof(T));

//        LOGD("chengke readAligned sizeof(T):%d mDataPos:%d", (int)sizeof(T), (int)mDataPos)
    if ((mDataPos+sizeof(T)) <= mDataSize) {
        const void* data = mData+mDataPos;
        mDataPos += sizeof(T);
        *pArg =  *reinterpret_cast<const T*>(data);
        return NO_ERROR;
    } else {
        return NOT_ENOUGH_DATA;
    }
}

template<class T>
T AndroidParcel::readAligned() const {
    T result;
    if (readAligned(&result) != NO_ERROR) {
        result = 0;
    }

    return result;
}

template<class T>
status_t AndroidParcel::writeAligned(T val) {
//    COMPILE_TIME_ASSERT_FUNCTION_SCOPE(PAD_SIZE_UNSAFE(sizeof(T)) == sizeof(T));

    if ((mDataPos+sizeof(val)) <= mDataCapacity) {
restart_write:
        *reinterpret_cast<T*>(mData+mDataPos) = val;
        return finishWrite(sizeof(val));
    }

    status_t err = growData(sizeof(val));
    if (err == NO_ERROR) goto restart_write;
    return err;
}

status_t AndroidParcel::readInt32(int32_t *pArg) const
{
    return readAligned(pArg);
}

int32_t AndroidParcel::readInt32() const
{
    return readAligned<int32_t>();
}

status_t AndroidParcel::readInt64(int64_t *pArg) const
{
    return readAligned(pArg);
}


int64_t AndroidParcel::readInt64() const
{
    return readAligned<int64_t>();
}

status_t AndroidParcel::readFloat(float *pArg) const
{
    return readAligned(pArg);
}

float AndroidParcel::readFloat() const
{
    return readAligned<float>();
}

status_t AndroidParcel::readDouble(double *pArg) const
{
    return readAligned(pArg);
}

double AndroidParcel::readDouble() const
{
    return readAligned<double>();
}

status_t AndroidParcel::readIntPtr(intptr_t *pArg) const
{
    return readAligned(pArg);
}


intptr_t AndroidParcel::readIntPtr() const
{
    return readAligned<intptr_t>();
}

const char* AndroidParcel::readCString() const
{
    const size_t avail = mDataSize-mDataPos;
    if (avail > 0) {
        const char* str = reinterpret_cast<const char*>(mData+mDataPos);
        // is the string's trailing NUL within the AndroidParcel's valid bounds?
        const char* eos = reinterpret_cast<const char*>(memchr(str, 0, avail));
        if (eos) {
            const size_t len = eos - str;
            mDataPos += pad_size(len+1);
//            ALOGV("readCString Setting data pos of %p to %zu", this, mDataPos);
            return str;
        }
    }
    return NULL;
}

const char16_t* AndroidParcel::readString16Inplace(size_t* outLen) const
{
    int32_t size = readInt32();
    // watch for potential int overflow from size+1
    if (size >= 0 && size < INT32_MAX) {
        *outLen = size;
        const char16_t* str = (const char16_t*)readInplace((size+1)*sizeof(char16_t));
        if (str != NULL) {
            return str;
        }
    }
    *outLen = 0;
    return NULL;
}


int32_t AndroidParcel::readExceptionCode() const
{
//    binder::Status status;
//    status.readFromAndroidParcel(*this);
//    return status.exceptionCode();
      int32_t exception_code = readAligned<int32_t>();
      if (exception_code == EX_HAS_REPLY_HEADER) {
        int32_t header_start = dataPosition();
        int32_t header_size = readAligned<int32_t>();
        // Skip over fat responses headers.  Not used (or propagated) in
        // native code
        setDataPosition(header_start + header_size);
        // And fat response headers are currently only used when there are no
        // exceptions, so return no error:
        return 0;
      }
      return exception_code;
}


const uint8_t* AndroidParcel::ipcData() const
{
    return reinterpret_cast<const uint8_t*>(mData);
}

size_t AndroidParcel::ipcDataSize() const
{
    return (mDataSize > mDataPos ? mDataSize : mDataPos);
}

void AndroidParcel::ipcSetDataForReader(const uint8_t* data, size_t dataSize, release_func relFunc)
{
    freeDataNoInit();
    mError = NO_ERROR;
    mData = const_cast<uint8_t*>(data);
    mDataSize = mDataCapacity = dataSize;
    mDataPos = 0;

    mOwner = relFunc;
}

void AndroidParcel::ipcSetDataForWriter(size_t desired)
{
    restartWrite(desired);
}

void AndroidParcel::freeData()
{
    freeDataNoInit();
    initState();
}

void AndroidParcel::freeDataNoInit()
{
    if (mOwner) {
        mOwner(this, mData, mDataSize);
    } else {
        if (mData) {
            free(mData);
            mData = NULL;
        }
    }
}

status_t AndroidParcel::growData(size_t len)
{
    if (len > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    size_t newSize = ((mDataSize+len)*3)/2;
//    LOGD("chengke growData %d %d", newSize, mDataSize);
    return (newSize <= mDataSize)
            ? (status_t) NO_MEMORY
            : continueWrite(newSize);
}

status_t AndroidParcel::restartWrite(size_t desired)
{
    if (desired > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    if (mOwner) {
        freeData();
        return continueWrite(desired);
    }

    if (!mData || mDataCapacity < desired) {
        uint8_t *data = (uint8_t *) realloc(mData, desired);
        if (!data && desired > mDataCapacity) {
            mError = NO_MEMORY;
            return NO_MEMORY;
        }

        if (data) {
            mData = data;
            mDataCapacity = desired;
        }
    }

    mDataSize = mDataPos = 0;
//    ALOGV("restartWrite Setting data size of %p to %zu", this, mDataSize);
//    ALOGV("restartWrite Setting data pos of %p to %zu", this, mDataPos);

    return NO_ERROR;
}

status_t AndroidParcel::continueWrite(size_t desired)
{
    if (desired > INT32_MAX) {
        // don't accept size_t values which may have come from an
        // inadvertent conversion from a negative int.
        return BAD_VALUE;
    }

    if (mOwner) {
        // If the size is going to zero, just release the owner's data.
        if (desired == 0) {
            freeData();
            return NO_ERROR;
        }

        // If there is a different owner, we need to take
        // posession.
        uint8_t* data = (uint8_t*)malloc(desired);
        if (!data) {
            mError = NO_MEMORY;
            return NO_MEMORY;
        }
        if (mData) {
            memcpy(data, mData, mDataSize < desired ? mDataSize : desired);
        }

        mOwner(this, mData, mDataSize);
        mOwner = NULL;

        mData = data;
        mDataSize = (mDataSize < desired) ? mDataSize : desired;
        mDataCapacity = desired;
    } else if (mData) {
        // We own the data, so we can just do a realloc().
        if (desired > mDataCapacity) {
            uint8_t* data = (uint8_t*)realloc(mData, desired);
            if (data) {
                mData = data;
                mDataCapacity = desired;
            } else if (desired > mDataCapacity) {
                mError = NO_MEMORY;
                return NO_MEMORY;
            }
        } else {
            if (mDataSize > desired) {
                mDataSize = desired;
//                ALOGV("continueWrite Setting data size of %p to %zu", this, mDataSize);
            }
            if (mDataPos > desired) {
                mDataPos = desired;
//                ALOGV("continueWrite Setting data pos of %p to %zu", this, mDataPos);
            }
        }

    } else {
//        LOGD("chengke continueWrite ccc")
        // This is the first data.  Easy!
        uint8_t* data = (uint8_t*)malloc(desired);
        if (!data) {
            mError = NO_MEMORY;
            return NO_MEMORY;
        }

        mData = data;
        mDataSize = mDataPos = 0;
//        ALOGV("continueWrite Setting data size of %p to %zu", this, mDataSize);
//        ALOGV("continueWrite Setting data pos of %p to %zu", this, mDataPos);
        mDataCapacity = desired;
    }

    return NO_ERROR;
}

void AndroidParcel::initState()
{
    mError = NO_ERROR;
    mData = 0;
    mDataSize = 0;
    mDataCapacity = 0;
    mDataPos = 0;
    mOwner = NULL;
}
