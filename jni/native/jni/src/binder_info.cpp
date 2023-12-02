#include "cmn_env.h"
#include "cmn_utils.h"
#include "binder_info.h"
#include "cmn_mutex.h"
#include <string.h>

// Note: must be kept in sync with android/os/StrictMode.java's PENALTY_GATHER
// defined in Parcel.cpp
#define STRICT_MODE_PENALTY_GATHER      0x100
#define STRICT_MODE_PENALTY_GATHER_6    0x400000// 6.0
#define STRICT_MODE_PENALTY_GATHER_10   0xc2000004// 10.0
#define STRICT_MODE_PENALTY_GATHER_11   0x80000000// 11.0
static uint32_t g_strict_mode_value = 0;

// --------------------------------------------------------------------------------------------------------------
TBinderTokenItem::TBinderTokenItem(const binder_txn_st* txn)
: tokenLen(0), tokenPtr(NULL), tokenHash(0) {
    if (__UNLIKELY(txn->data_size < __MIN_TXN_TOKEN_DATA_SIZE__)) return ;
    const int32_t* ptr = reinterpret_cast<const int32_t*>(txn->data.ptr.buffer);
    if (0 == (*ptr & g_strict_mode_value)) {
        return ;
    }
    ++ ptr;// skip strict model
    if (ENV_SDK_INT_Q_10_29 <= CmnEnv::sdkVersion()) ++ ptr;// 10.0 <=, skip flags(ff ff ff ff)
    if (ENV_SDK_INT_R_11_30 <= CmnEnv::sdkVersion() && ENV_BRAND_HUAWEI != CmnEnv::brand()) ++ ptr;// Not for HuaWei 11.0

    int32_t len = *ptr;
    if (__LIKELY(3 < len && len < 256 && len < (txn->data_size-sizeof(int)-sizeof(int)))) {// valid check
        tokenLen = len;
        tokenPtr = ++ ptr;
        tokenHash = utils_hash16((const uint16_t*)(ptr+1), tokenLen);
    }
}

void TBinderTokenItem::initEnv() {
    if (__LIKELY(0 == g_strict_mode_value)) {
        int ver = CmnEnv::sdkVersion();
        if (ENV_SDK_INT_R_11_30 <= ver) {
            g_strict_mode_value = STRICT_MODE_PENALTY_GATHER_11;
        } else if (ENV_SDK_INT_Q_10_29 <= ver) {
            g_strict_mode_value = STRICT_MODE_PENALTY_GATHER_10;
        } else {
            g_strict_mode_value = STRICT_MODE_PENALTY_GATHER_6;
        }
    }
}

bool TBinderTokenItem::isTargetToken(uint16_t* token, int32_t len) {
    if (len == tokenLen && token && tokenPtr) {
        return 0 == memcmp(token, tokenPtr, len);
    }
    return false;
}

__DEF_AUTO_RELEASE_MUTEX(binder_token_log);
void TBinderTokenItem::print(const binder_txn_st* txn, bool oneway) const {
    if (isValid()) {
        __AUTO_RELEASE_MUTEX(binder_token_log);
        char tag[100];
        sprintf(tag, "Binder::%s: 0x%x, %d", oneway? "owy" : "tgt", tokenHash, txn->code);
        utils_print_string16(tag, tokenPtr, tokenLen);
    } else {
        LOGD("Binder::%s: null, %d", oneway? "owy" : "tgt", txn->code)
    }
}

void binder_dump_txn(binder_txn_st *txn) {
    binder_size_t *offs = (binder_size_t *)txn->data.ptr.offsets;
    unsigned count = txn->offsets_size / sizeof(binder_size_t);
    if (txn->flags & TF_ONE_WAY) {
        LOGD("target:%8x   cookie:%8x   code:%8d   flags:   0x%x(ONE WAY)", (int)(txn->target.handle), (int)(txn->cookie), (int)(txn->code), (int)(txn->flags))
    } else {
        LOGD("target:%8x   cookie:%8x   code:%8d   flags:   0x%x(READ REPLY)", (int)(txn->target.handle), (int)(txn->cookie), (int)(txn->code), (int)(txn->flags))
    }
    LOGD("   pid:%8d      uid:%8d   size:%8d    offs:%d", txn->sender_pid, txn->sender_euid, (int)txn->data_size, (int)txn->offsets_size)
    utils_hexdump((void*)txn->data.ptr.buffer, txn->data_size);

    binder_flat_binder_object *obj = NULL;
    while (count--) {
        obj = (binder_flat_binder_object*) (((char*)txn->data.ptr.buffer) + *offs);
        LOGD("binder object offs:0x%x  type:0x%x  flags:0x%x  ptr:0x%x  cookie:0x%x",
             (int)(*offs), (int)(obj->hdr.type), (int)(obj->flags), (int)(__BINDER_OBJ_VALUE__(obj)), (int)(obj->cookie)
              )
        ++ offs;
    }
}

void binder_dump_bwr(struct binder_write_read* bwr) {
    LOGE("read_buffer:%p, read_consumed:%ld, read_size:%ld", (void*)bwr->read_buffer, (long)bwr->read_consumed, (long)bwr->read_size)
    utils_hexdump((void*)bwr->read_buffer, bwr->read_size);
    LOGE("write_buffer:%p, write_consumed:%ld, write_size:%ld", (void*)bwr->write_buffer, (long)bwr->write_consumed, (long)bwr->write_size)
    utils_hexdump((void*)bwr->write_buffer, bwr->write_size);
}
