#include <string.h>
#include "binderceptor_debug.h"
#include "binder_parcel.h"
#include "cmn_utils.h"

// ----------------------------------------------------------------------------------------------
void BinderceptorDebug::onFrameworkBinderBefore(TBinderInfo* pInfo) {
    // TODO ...
}

static bool doBinderFilter(binder_txn_st* txn, TBinderTokenItem& token, bool oneway, void* keyData, size_t keyDatalen) {
    if ((txn->data_size < keyDatalen + sizeof(int32_t))) return false;

//    BinderParcel* pr = __PARCEL_NEW_DATA((const uint8_t *) txn->data.ptr.buffer, txn->data_size);
//    if (__UNLIKELY(!pr)) return false;
//    __PARCEL_AUTO_FREE(pr);
//    if (__UNLIKELY(0 != pr->skipToken())) return false;// token
//    size_t len = 0;
//    const char16_t* pkg16 = pr->readString16Inplace(&len);
//    if (len < 3 || !pkg16) return false;
//
//#if __ENABLE_LOG_VALUE__
//    utils_print_string16(tag, pkg16, len);
//#endif

    const uint8_t *ptr = (const uint8_t *) txn->data.ptr.buffer;
    for (int i=0, len = txn->data_size-keyDatalen-sizeof(int32_t); i <= len; ++i) {
        uint32_t* intPtr = (uint32_t*)(ptr + i);
        if (*intPtr != 26) continue;
        if (memcmp(ptr + i + sizeof(int32_t), keyData, keyDatalen) != 0) continue;
        token.print(txn, oneway);
    }

    return true;
}

bool BinderceptorDebug::onFrameworkBinderTarget(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;

    LOGD("BinderceptorDebug: target2 %p", txn)
    token.print(txn, false);
//    void* keyData = NULL;
//    size_t keyDatalen = 0;
//    doBinderFilter(txn, token, false, keyData, keyDatalen);

    return false;
}

void BinderceptorDebug::onFrameworkBinderAfterFaker(BinderParcel& reply, TBinderInfo& pInfo) {
    // TODO ...
}

void BinderceptorDebug::onFrameworkBinderAfterFixer(binder_txn_st* txn, TBinderInfo& pInfo) {
    // TODO ...
}

bool BinderceptorDebug::onFrameworkBinderOneway(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;

    LOGD("BinderceptorDebug: oneway2 %p", txn)
    token.print(txn, true);
//    void* keyData = NULL;
//    size_t keyDatalen = 0;
//    doBinderFilter(txn, token, true, keyData, keyDatalen);
    return false;
}
