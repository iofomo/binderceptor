/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BINDER_ANDROID_PARCEL_H__
#define __BINDER_ANDROID_PARCEL_H__

#include <stddef.h>
#include <stdint.h>
#include "cmn_errors.h"

//typedef uint16_t    char16_t;
typedef int32_t     status_t;

template <typename T> class LightFlattenable;


class AndroidParcel
{
public:
                        AndroidParcel();
                        ~AndroidParcel();

    typedef void        (*release_func)(AndroidParcel* parcel, const uint8_t* data, size_t dataSize);
    void                ipcSetDataForWriter(size_t desired);
    void                ipcSetDataForReader(const uint8_t* data, size_t dataSize, release_func relFunc);

    const uint8_t*      data() const;
    size_t              dataSize() const;
    size_t              dataAvail() const;
    size_t              dataPosition() const;
    size_t              dataCapacity() const;

    status_t            setDataSize(size_t size);
    void                setDataPosition(size_t pos) const;
    status_t            setDataCapacity(size_t size);
    
    status_t            setData(const uint8_t* buffer, size_t len);

    status_t            appendFrom(const AndroidParcel *parcel,
                                   size_t start, size_t len);
    status_t            appendFrom(uint8_t *_data, size_t offset, int len);

    void                freeData();

    status_t            errorCheck() const;
    void                setError(status_t err);
    
    status_t            write(const void* data, size_t len);
    void*               writeInplace(size_t len);
    status_t            writeUnpadded(const void* data, size_t len);
    status_t            writeInt32(int32_t val);
    status_t            writeInt64(int64_t val);
    status_t            writeFloat(float val);
    status_t            writeDouble(double val);
    status_t            writeIntPtr(intptr_t val);
    status_t            writeCString(const char* str);

    template<typename T>
    status_t            write(const LightFlattenable<T>& val);

    status_t            writeNoException();

    void                remove(size_t start, size_t amt);
    
    status_t            read(void* outData, size_t len) const;
    const void*         readInplace(size_t len) const;
    int32_t             readInt32() const;
    status_t            readInt32(int32_t *pArg) const;
    int64_t             readInt64() const;
    status_t            readInt64(int64_t *pArg) const;
    float               readFloat() const;
    status_t            readFloat(float *pArg) const;
    double              readDouble() const;
    status_t            readDouble(double *pArg) const;
    intptr_t            readIntPtr() const;
    status_t            readIntPtr(intptr_t *pArg) const;

    const char*         readCString() const;
    const char16_t*     readString16Inplace(size_t* outLen) const;

    template<typename T>
    status_t            read(LightFlattenable<T>& val) const;

    int32_t             readExceptionCode() const;

    const uint8_t*      ipcData() const;
    size_t              ipcDataSize() const;

private:
    AndroidParcel(const AndroidParcel& o);
    AndroidParcel& operator=(const AndroidParcel& o);

    status_t            finishWrite(size_t len);
    status_t            growData(size_t len);
    status_t            restartWrite(size_t desired);
    status_t            continueWrite(size_t desired);
    void                freeDataNoInit();
    void                initState();

    template<class T>
    status_t            readAligned(T *pArg) const;

    template<class T>   T readAligned() const;

    template<class T>
    status_t            writeAligned(T val);

    status_t            mError;
    uint8_t*            mData;
    size_t              mDataSize;
    size_t              mDataCapacity;
    mutable size_t      mDataPos;

    release_func        mOwner;
};

// ---------------------------------------------------------------------------

template<typename T>
status_t AndroidParcel::write(const LightFlattenable<T>& val) {
    size_t size(val.getSize());
    if (!val.isFixedSize()) {
        status_t err = writeInt32(size);
        if (err != NO_ERROR) {
            return err;
        }
    }
    if (size) {
        void* buffer = writeInplace(size);
        return buffer == NULL ? NO_MEMORY :
                val.flatten(buffer);
    }
    return NO_ERROR;
}

template<typename T>
status_t AndroidParcel::read(LightFlattenable<T>& val) const {
    size_t size;
    if (val.isFixedSize()) {
        size = val.getSize();
    } else {
        int32_t s;
        status_t err = readInt32(&s);
        if (err != NO_ERROR) {
            return err;
        }
        size = s;
    }
    if (size) {
        void const* buffer = readInplace(size);
        return buffer == NULL ? NO_MEMORY :
                val.unflatten(buffer, size);
    }
    return NO_ERROR;
}

#endif // __BINDER_ANDROID_PARCEL_H__
