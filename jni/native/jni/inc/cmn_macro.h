#ifndef __CMN_MACRO_H__
#define __CMN_MACRO_H__

#define __PATH_MAX__            1024
#define __BUFFER_MAX__          4096
#define __1_K_SIZE_VALUE__      1024
#define __1_M_SIZE_VALUE__      1048576//1024*1024
#define __10_M_SIZE_VALUE__     10485760//1024*1024*10
#define MEM_FREE(p) if(p){ free(p); p=0; }
#define MEM_DELETE(p) if(p){ delete p; p=0; }

#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif// end of NELEM

#define __COMPARE_STRING(p, q, l) utils_cmp_reverse_8((const uint8_t*)(p), (const uint8_t*)(q), l)

#define __LIKELY(x)       __builtin_expect(!!(x), true)
#define __UNLIKELY(x)     __builtin_expect(!!(x), false)

#define __PRINTSTR(p) (p?(const char*)p:"")

#define __IS_EMPTY_STRING(s) (NULL == s || '\0' == s[0])

#define __CMN_IS_DIR(s) (S_ISDIR((s).st_mode))
#define __CMN_IS_FILE(s) (S_ISREG((s).st_mode))

typedef struct {
    char* str;
    unsigned int strLen;// strlen(str)
} TStringItem;
#define __MEM_FREE_STRING_ITEM(s) { if (s.str) { free(s.str); s.str = NULL; } s.strLen = 0; }

typedef struct {
    const char* str;
    unsigned int strLen;// strlen(str)
} TStringCItem;
const TStringCItem __G_STRINGCITEM_NULL = { 0 };
#define __CMP_CSTRING_(p, q) (p.strLen == q.strLen && 0 == memcmp(p.str, q.str, q.strLen))

typedef struct {
    unsigned int hash;
    unsigned int flag;
} TStringHash;

#define __CMP_STRING_HASH(p, q) (p.hash == q.hash && p.flag == q.flag)
#define __UNCMP_STRING_HASH(p, q) (p.hash != q.hash || p.flag != q.flag)
#define __DEF_SH_NAME(n, h, f) static const TStringHash g_def_sh_##n={h, f}
#define __SH_NAME(n) g_def_sh_##n
#define __CMP_SH(n, q) (g_def_sh_##n.hash == q.hash && g_def_sh_##n.flag == q.flag)

#define __TEXT_EMPTY(t) (!t || '\0' == t[0])// null or ""

// defined in "/system/core/include/cutils/properties.h"
#ifndef PROPERTY_VALUE_MAX
#define PROPERTY_VALUE_MAX  92
#endif

#define __ENABLE_LOG_D__        1
#define __ENABLE_LOG_W__        1
#define __ENABLE_LOG_E__        1
#define __ENABLE_ASSERT__       1

void utils_logger(const char* tag, const char* fmt, ...);

#ifdef __ENABLE_LOG_D__
#define LOGD(...) utils_logger("LOGD", __VA_ARGS__);
#else
#define LOGD(...)
#endif

#ifdef __ENABLE_LOG_W__
#define LOGW(...) utils_logger("LOGW", __VA_ARGS__);
#else
#define LOGW(...)
#endif

#ifdef __ENABLE_LOG_W__
#define LOGE(...) utils_logger("LOGE", __VA_ARGS__);
#else
#define LOGE(...)
#endif

#ifdef __ENABLE_ASSERT__
#define __ASSERT(p) if (!(p)) { utils_logger("LOGASSERT", "assert:[%d]%s", __LINE__, __FILE__); abort(); }
#else
#define __ASSERT(p)
#endif


#endif // end of __CMN_MACRO_H__
