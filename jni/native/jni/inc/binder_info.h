#ifndef __BINDER_INFO_H__
#define __BINDER_INFO_H__

#include <stdlib.h>
#include <stdint.h>
#include "binder_def.h"
#include "binder_linux.h"
#include "cmn_macro.h"

//typedef uint16_t char16_t;
class TBinderDataBase {
public:
    TBinderDataBase(){}
    virtual ~TBinderDataBase() {}

#ifdef __ENABLE_LOG_VALUE__
    virtual void print() const {}
#endif
    virtual const char* desc8() const { return NULL; }
    virtual const char16_t* desc16(unsigned int& len) const { return NULL; }
    virtual bool isValid() const { return false; }
};

class TBinderInfo {
public:
    uint32_t type;   //@see##TBinderSvrType
    unsigned int method;   //@see##TBinderMethodType
    unsigned int action; //@see##TBinderIoctlActionType

    unsigned int flag;// BC_TRANSACTION, BC_REPLY, BR_TRANSACTION, BR_REPLY, fromCache, TPermissionControl
    uintptr_t data;// data pointer or int value
    unsigned int data_size;
    void* txnPtrBC;
    void* txnPtrBR;

    TBinderInfo():type(0)
    , method(0), action(0), flag(0), data(0), data_size(0)
    , txnPtrBC(NULL), txnPtrBR(NULL), mCallCount(0)
    {}

    ~TBinderInfo() {// do not use virtual property
        releaseData();
    }

    inline void setData(binder_uintptr_t _data) {
        releaseData();
        data = _data; data_size = 0;
    }
    inline void setData(TBinderDataBase* _data, unsigned int _data_size) {
        releaseData();
        data_size = _data_size;
        data = (uintptr_t)_data;
    }
    inline bool isDataValue() const { return 0 != data && 0 == data_size; }
    inline bool isDataValue(binder_uintptr_t val) const { return val == data && 0 == data_size; }
    inline bool isDataPointer(unsigned int classSize) const {
        return 0 != data && 0 < classSize && classSize == data_size;
    }

    // flag bit information
    inline bool isNew() const { return 0 == (flag & 0x1); }//fromCache(0x1)
    inline void enableCache() { flag |= 0x1; txnPtrBC = txnPtrBR = NULL; }
    inline void addCmd(bool bc) { flag |= bc ? 0x2 : 0x4; }
    inline bool isCmdBC() const { return 0 != (flag & 0x2); }//BC_REPLY or BC_TRANSACTION
    inline bool isCmdBR() const { return 0 != (flag & 0x4); }//BR_REPLY or BR_TRANSACTION or BR_TRANSACTION_COMPLETE
    inline bool isFinished() const {
        return isNoAction(EIoctlAction_After) || !isCmdBC() || isCmdBR();
    }

    inline void addAction(unsigned int _action) { action |= _action; }
    inline bool isAction(unsigned int _action) const { return 0 != (_action & action); }
    inline bool isNoAction(unsigned int _action) const { return 0 == (_action & action); }
//    inline void removeAction(unsigned int _action) { action &= ~_action; }
    inline bool isValid() const { return 0 != type && 0 != method; }

    inline const char* desc8() const {
        if (data && 0 < data_size) {
            return ((TBinderDataBase*)data)->desc8();
        }
        return NULL;
    }

    inline const char16_t* desc16(unsigned int& len) const {
        if (data && 0 < data_size) {
            return ((TBinderDataBase*)data)->desc16(len);
        }
        return NULL;
    }

    inline void setTxnBC(void* txn) { txnPtrBC = txn; }
    inline void setTxnBR(void* txn) { txnPtrBR = txn; }

    inline void reset() {
        releaseData();
        method = 0;
        action = 0; flag = 0;
        type = 0;
        txnPtrBC = txnPtrBR = NULL;
    }

    inline bool isCallIgnore() { return 0 < mCallCount; }
    inline void callInc() { ++ mCallCount; }
    inline void callDec() {
        __ASSERT(0 < mCallCount)
        if (__LIKELY(0 < mCallCount)) -- mCallCount;
    }

#ifdef __ENABLE_LOG_VALUE__
    void print() const {
        LOGD("BinderInfo::p t:%s(%d), m:%d, a:0x%x, f:0x%x", BinderUtils::binder_string_of_type(type), type, method, action, flag)
        LOGD("BinderInfo::p n:%d", isNew())
        LOGD("BinderInfo::p bc:%p, br:%p", txnPtrBC, txnPtrBR)
        if (0 == data_size) {
            LOGD("BinderInfo::p d:%u", data)
        } else {
            LOGD("BinderInfo::p d:0x%x, s:%u", data, data_size)
        }
        if (0 != data && 0 < data_size) {
            ((TBinderDataBase*)data)->print();
        }
    }
#endif

protected:
    inline void releaseData() {
        if (0 != data && 0 < data_size) {
            delete (TBinderDataBase*)(data);
        }
        data = 0;
        data_size = 0;
    }

private:
    unsigned int mCallCount;// ioctl call count
};


class TBinderTokenItem {
public:
    TBinderTokenItem(const binder_txn_st* txn);

    inline bool equal(int len, unsigned hash) const {
        return tokenHash == hash && tokenLen == len;
    }

    inline bool unequal(int len, unsigned hash) const {
        return tokenHash != hash || tokenLen != len;
    }

    inline bool isValid() const { return 0 != tokenHash && 0 < tokenLen; }
    inline bool isInvalid() const { return 0 == tokenHash || tokenLen <= 0; }
    inline unsigned getHash() const { return tokenHash; }
    inline int32_t getLength() const { return tokenLen; }

    bool isTargetToken(uint16_t* token, int32_t len);

    static void initEnv();

#ifdef __ENABLE_LOG_D__
    void print(const binder_txn_st* txn, bool oneway) const;
#endif

private:
    int32_t tokenLen;
    const int32_t* tokenPtr;
    unsigned tokenHash;
};

void binder_dump_txn(binder_txn_st *txn);
void binder_dump_bwr(struct binder_write_read* bwr);

#endif// end of __BINDER_PARSE_INFO_H__
