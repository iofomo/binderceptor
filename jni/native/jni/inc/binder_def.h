#ifndef __BINDER_DEF_H__
#define __BINDER_DEF_H__

#include "cmn_macro.h"

typedef enum {
    EIoctlAction_None           = 0x0,// do nothing
    EIoctlAction_Before_Fixer   = 0x1,// fix the arguments(txn) before call ioctl
    EIoctlAction_After_Fixer    = 0x2,// after call ioctl, then fix or parse the reply data
    EIoctlAction_After_Faker    = 0x4,// after call ioctl, then reply fake binder_txn, drop all then BC
    EIoctlAction_After          = EIoctlAction_After_Fixer|EIoctlAction_After_Faker,
} TBinderIoctlActionType;

// define macro methods
#define __COMPARE(str8, str16, len) (len == __LENGTH(str8) && utils_cmp_reverse((const uint8_t*)__NAME(str8), (const uint16_t*)(str16), len))
#define __COMPARE8(str1, str2, len) (len == __LENGTH(str1) && utils_cmp_reverse_8((const uint8_t*)__NAME(str1), (const uint8_t*)(str2), len))

#define __TMP_NAME(name) tmp_def_##name
#define __TMP_LENGTH(name) tmp_def_len_##name
#define __DEF_TMP_NAME(name) const char* tmp_def_##name
#define __DEF_TMP_LENGTH(name) const size_t tmp_def_len_##name = strlen(tmp_def_##name)
#define __COMPARE_TMP(str8, str16, len) (len == __TMP_LENGTH(str8) && utils_cmp_reverse((const uint8_t*)__TMP_NAME(str8), (uint16_t*)(str16), len))

#define __STRONG_BINDER_SIZE__  sizeof(binder_flat_binder_object)// size of strong binder(4 int)
const unsigned int __MIN_PARCEL_SIZE__ = sizeof(int32_t) + sizeof(int32_t);

#define __BINDER_OBJ_VALUE__(p) (BINDER_TYPE_HANDLE==p->hdr.type?(binder_uintptr_t)p->handle:(binder_uintptr_t)p->binder)
#define __MAX_BINDER_ONEWAY_VAL__   0xffff

#define __MIN_TXN_TOKEN_DATA_SIZE__   (sizeof(int32_t)+sizeof(int32_t)+sizeof(int32_t))

#define __MAX_BINDER_TOKEN_LENGTH__     256
#define __IS_VALID_BINDER_TOKEN(ptr, len) (ptr && 0 < len && len < __MAX_BINDER_TOKEN_LENGTH__)
#define __IS_INVALID_BINDER_TOKEN(ptr, len) (!ptr || len <= 0 || __MAX_BINDER_TOKEN_LENGTH__ <= len)

#define SYSTEM_LIB_BINDER   "libbinder.so"

#endif// end of __BINDER_DEF_H__
