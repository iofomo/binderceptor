#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <paths.h>
#include <stdarg.h>
#include <dirent.h>
#include <cctype>
#include <android/log.h>
#include "cmn_macro.h"
#include "cmn_utils.h"

//-------------------------------------------------------------------------------------------------------------------------
void utils_logger(const char* tag, const char* fmt, ...) {
    char buf[1024];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, 1020, fmt, ap);
    va_end(ap);

    __android_log_write(5, tag, buf);
}

void utils_hexdump(const void *_data, unsigned size) {
    if (!_data) return ;

    int len;
    size_t offset = 0;
    char tmp[32];
    char line[1024];
    const uint8_t *data = (const uint8_t *)_data;
    while (offset < size) {
        memset(line, 0, sizeof(line));
        len = sprintf(tmp, "%08lx:  ", (unsigned long)offset);

        memcpy(line, tmp, len);

        for (int i = 0; i < 16; ++i) {
            if (i == 8) {
                strcat(line, " ");
            }
            if (offset + i >= size) {
                strcat(line, "   ");
            } else {
                sprintf(tmp, "%02x ", data[offset + i]);
                strcat(line, tmp);
            }
        }

        strcat(line, " ");

        for (int i = 0; i < 16; ++i) {
            if (offset + i >= size) {
                break;
            }

            if (__UTILS_IS_PRINT(data[offset + i])) {
                line[strlen(line)] = (char)data[offset + i];
            } else {
                line[strlen(line)] =  '.';
            }
        }
        LOGE("%s", line)
        offset += 16;
    }
}

bool utils_memcmp_16(const uint16_t *buffer1, const uint16_t *buffer2, size_t count) {
    if (__UNLIKELY(!count)) return true;
    if (__UNLIKELY(!buffer1 || !buffer2)) return false;
    while (*buffer1++ == *buffer2++ && --count) ;
    return 0 == count;
}

bool utils_cmp(const uint8_t *buffer1, const uint16_t *buffer2, size_t count) {
    if (__UNLIKELY(!count)) return true;
    if (__UNLIKELY(!buffer1 || !buffer2)) return false;
    while (0 < count --) {
        if (*buffer1++ == *buffer2++) continue;
        return false;
    }
    return true;
}

bool utils_cmp_reverse(const uint8_t *buffer1, const uint16_t *buffer2, size_t count) {
    if (__UNLIKELY(!count)) return true;
    if (__UNLIKELY(!buffer1 || !buffer2)) return false;
    buffer1 += count - 1;
    buffer2 += count - 1;
    while (*buffer1-- == *buffer2-- && -- count) ;
    return 0 == count;
}

bool utils_cmp_reverse_8(const uint8_t* buffer1, const uint8_t* buffer2, size_t count) {
    if (__UNLIKELY(!count)) return true;
    if (__UNLIKELY(!buffer1 || !buffer2)) return false;
    buffer1 += count - 1;
    buffer2 += count - 1;
    while (*buffer1-- == *buffer2-- && --count) ;
    return 0 == count;
}

bool utils_cmp_reverse_8_ignore_case(const uint8_t* buffer1, const uint8_t* buffer2, size_t count) {
    if (!count) return true;
    if (!buffer1 || !buffer2) return false;
    buffer1 += count - 1;
    buffer2 += count - 1;
    while (tolower(*buffer1--) == tolower(*buffer2--) && --count) ;
    return 0 == count;
}

bool utils_cmp_reverse_16(const uint16_t* buffer1, const uint16_t * buffer2, size_t count) {
    if (__UNLIKELY(!count)) return true;
    if (__UNLIKELY(!buffer1 || !buffer2)) return false;
    buffer1 += count - 1;
    buffer2 += count - 1;
    while (*buffer1-- == *buffer2-- && --count) ;
    return 0 == count;
}

void utils_print_string16(const char* tag, const void* _data, size_t len) {
    char buffer[1024] = {0};

    if (tag) {
        strcpy(buffer, tag);
        strcat(buffer, ": ");
    }
    if (_data) {
        size_t dataIndex = 0;
        size_t bufferIndex = strlen(buffer);
        const uint8_t *data = (const uint8_t *)_data;

        len *= 2;
        while (dataIndex < len) {
            if (__UTILS_IS_PRINT(data[dataIndex])) {
                buffer[bufferIndex] = (char)data[dataIndex];
            } else {
                buffer[bufferIndex] =  '.';
            }
            dataIndex += 2;
            bufferIndex += 1;
        }
    }
    LOGW("%s", buffer)
}

long long utils_nano_time() {
    // from source code: java_lang_System.cpp
#if 1//defined(HAVE_POSIX_CLOCKS)
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000LL + now.tv_nsec / 1e6;
#else
    timeval now;
    gettimeofday(&now, NULL);
    return static_cast<long long>(now.tv_sec) * 1000000000LL + now.tv_usec * 1000LL;
#endif
}

long long utils_current_time_millis() {
    timeval now;
    gettimeofday(&now, NULL);
    long long when = now.tv_sec * 1000LL + now.tv_usec / 1000;
    return when;
}

long long utils_current_time_seconds() {
    timeval now;
    gettimeofday(&now, NULL);
    long long when = now.tv_sec;
    return when;
}

void* utils_dlsym(const char* libName, const char* funcName) {
    void* funcPtr = NULL;
    void* handle = dlopen(libName, RTLD_NOW);
    if (__LIKELY(handle)) {
        funcPtr = dlsym(handle, funcName);
        dlclose(handle);
    } else {
        LOGE("dlsym: %s, %s, %d, %s", libName, funcName, errno, strerror(errno))
        __ASSERT(0)
    }
    return funcPtr;
}

void* utils_dlsym_global(const char* libName, const char* funcName) {
    void* funcPtr = NULL;
    void* handle = dlopen(libName, RTLD_LAZY|RTLD_GLOBAL);
    if (__LIKELY(handle)) {
        funcPtr = dlsym(handle, funcName);
    } else {
        LOGE("dlsym: %s, %s, %d, %s", libName, funcName, errno, strerror(errno))
        __ASSERT(0)
    }
    return funcPtr;
}

void* utils_dlsym_ex(const char* libName, const char** names, int nameLen) {
    __ASSERT(libName && names && 0 < nameLen)

    void* funcPtr = NULL;
    void* handle = dlopen(libName, RTLD_LAZY);
    if (__LIKELY(handle)) {
        for (int i=0; i<nameLen; ++i) {
            funcPtr = dlsym(handle, names[i]);
            if (funcPtr) break;
        }
        dlclose(handle);
    }
    return funcPtr;
}

bool utils_dlsyms(const char* libName, cmnDLSYM_API_FUN* funcs, int length) {
    void* handle = dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
    if (__LIKELY(handle)) {
        void* fun = NULL;
        for (int i=0; i<length; ++i) {
            fun = dlsym(handle, funcs[i].funName);
            if (!fun) {
                LOGE("utils_dlsyms dlsym fail: %s, %s, %d, %s", libName, funcs[i].funName, errno, strerror(errno))
                break;
            }
            *(funcs[i].funPtr) = fun;
        }
        dlclose(handle);
        return NULL != fun;
    }
    LOGE("utils_dlsyms dlopen fail: %s, %d, %s", libName, errno, strerror(errno))
    return false;
}

size_t utils_str16len(const uint16_t *str) {
    size_t len = 0;
    if (__LIKELY(str)) {
        while (*str ++) ++len;
    }
    return len;
}

char* utils_malloc_string(const char* _data, int _pos, int _len) {
    char* str = NULL;
    if (__LIKELY(_data && 0 <= _pos && 0 <= _len)) {
        str = (char*)malloc(_len + 1);
        if (__LIKELY(str)) {
            if (__LIKELY(0 < _len)) {
                memcpy(str, _data + _pos, _len);
            }
            str[_len] = '\0';
        }
    }
    return str;
}

size_t utils_set_string(char*& desVal, const char* val) {
    size_t len = 0;
    MEM_FREE(desVal)
    if (__LIKELY(val)) {
        desVal = strdup(val);
    }
    return len;
}

const char* utils_strnstr(const char* src, size_t srcLen, const char* substr) {
    if (__UNLIKELY(!src || srcLen <= 0 || !substr)) return NULL;
    size_t subLen = strlen(substr);
    if (__UNLIKELY(subLen <= 0 || srcLen < subLen)) return NULL;

    int tmpLen = 0;
    srcLen -= subLen;
    while (tmpLen <= srcLen) {
        if (*src == *substr && strncmp(src, substr, subLen) == 0) {
            return src;
        }
        ++ tmpLen;
        ++ src;
    }

    return NULL;
}

char* utils_string_sub(const char *buf, const char *head, const char *end) {
    if (__UNLIKELY(!buf || !head || !end)) return NULL;

    const char* headBuf = strstr(buf, head);
    if (__UNLIKELY(!headBuf)) return NULL;

    const char* endBuf = strstr(headBuf, end);
    if (__UNLIKELY(!endBuf)) return NULL;

    size_t headLen = strlen(head);
    size_t len = endBuf - headBuf - headLen;

    char* str = (char *) malloc(len + 1);
    if (__LIKELY(str)) {
        strncpy(str, headBuf + headLen, len);
        str[len] = '\0';
    }
    return str;
}

int utils_string_strip(char* str) {
    if (__UNLIKELY(!str)) return 0;

    int cnt = 0;
    char* p = str;
    while (*str) {
        switch (*str) {
        case '\r': case '\n':
        case '\t': case ' ':
            ++ cnt;
            break;
        default:
            if (p != str) {
                *p = *str;
            }
            ++ p;
            break;
        }
        ++ str;
    }
    *p = '\0';
    return cnt;
}

int utils_string_strip(char* str, char ch) {
    if (__UNLIKELY(!str)) return 0;

    int cnt = 0;
    char* p = str;
    while (*str) {
        if (ch == *str) {
            ++cnt;
        } else {
            if (p != str) {
                *p = *str;
            }
            ++ p;
        }
        ++ str;
    }
    *p = '\0';
    return cnt;
}

int utils_string_lstrip(char* str) {
    if (__UNLIKELY(!str)) return 0;

    int cnt = 0;
    char* p = str;
    while (*str) {
        if (' ' == *str || '\r' == *str ||
            '\n' == *str || '\t' == *str
            ) {
            ++ cnt;
            ++ str;
            continue;
        }
        break;
    }
    if (p != str) {
        strcpy(p, str);
    }
    return cnt;
}

int utils_string_lstrip(char* str, char ch) {
    if (__UNLIKELY(!str)) return 0;

    int cnt = 0;
    char* p = str;
    while (*str) {
        if (ch == *str) {
            ++ cnt;
            ++ str;
            continue;
        }
        break;
    }
    if (p != str) {
        strcpy(p, str);
    }
    return cnt;
}

int utils_string_rstrip(char* str, size_t len) {
    if (__UNLIKELY(!str)) return 0;

    int cnt = 0;
    char* p = str + len;
    while (str != p--) {
        if (' ' == *p || '\r' == *p ||
            '\n' == *p || '\t' == *p
            ) {
            ++ cnt;
            *p = '\0';
            continue;
        }
        break;
    }
    return cnt;
}

int utils_string_rstrip(char* str, size_t len, char ch) {
    if (__UNLIKELY(!str)) return 0;

    int cnt = 0;
    char* p = str + len;
    while (str != p--) {
        if (ch == *p) {
            ++ cnt;
            *p = '\0';
            continue;
        }
        break;
    }
    return cnt;
}

void utils_string_lower(char* s) {
    if (!s) return;
    while (*s) {
        if ('A' <= *s && *s <= 'Z') *s += 32;
        ++ s;
    }
}

void utils_string_upper(char* s) {
    if (!s) return;
    while (*s) {
        if ('a' <= *s && *s <= 'z') *s -= 32;
        ++ s;
    }
}

int utils_string_split(char* src, char ch, char* items[], int itemsLen) {
    __ASSERT(src && items)
    if (!src || !items) return 0;

    int i = 0, cnt = 0;
    memset(items, 0, itemsLen*sizeof(char*));
    while (i < itemsLen && *src) {
        if (!items[i]) {
            ++ cnt;
            items[i] = src;
        }
        if (*src == ch) {
            ++ i;
            *src = '\0';
        }
        ++ src;
    }
    return cnt;
}

int utils_string_replace(char* str, char src, char des) {
    if (!str) return 0;

    int cnt = 0;
    while (*str) {
        if (*str == src) {
            *str = des;
            ++ cnt;
        }
        ++ str;
    }
    return cnt;
}

char* utils_convert_char16(const uint16_t* _data16, int _dataLen) {
    if (__UNLIKELY(!_data16 || _dataLen < 0)) return NULL;

    char* str8 = (char*) malloc(_dataLen*sizeof(char) + 1);
    if (__UNLIKELY(!str8)) return NULL;

    const char* data8 = (const char *)_data16;
    for (int i=0, j=0; i<_dataLen; i+=1, j+=sizeof(uint16_t)) {
        str8[i] = data8[j];
    }
    str8[_dataLen] = '\0';
    return str8;
}

void utils_convert_char16(const uint16_t* _data16, char* _data8/*OUT*/, int _dataLen) {
    if (__UNLIKELY(_dataLen <= 0)) return ;
    if (__UNLIKELY(!_data16 || !_data8)) {
        _data8[0] = '\0';
        return ;
    }

    const char* data8 = (const char *)_data16;
    for (int i=0, j=0; i<_dataLen; i+=1, j+=sizeof(uint16_t)) {
        _data8[i] = data8[j];
    }
    _data8[_dataLen] = '\0';
}

uint16_t* utils_char_to_uint16(const char* data8) {
    if (__UNLIKELY(!data8)) return NULL;

    size_t len = strlen(data8);
    size_t size16 = (len + 1) * sizeof(uint16_t);
    char* data16 = (char*)malloc(size16);
    if (__LIKELY(data16)) {
        memset(data16, 0, size16);
        for (int i = 0, j = 0; i < len; i += 1, j += sizeof(uint16_t)) {
            data16[j] = data8[i];
        }
    }
    return (uint16_t*)data16;
}

int utils_search_string16(const uint8_t* data, int dataLen, const uint16_t* tData, int tDataLen, int* posList/*out*/, int maxPosCnt) {
    if (__UNLIKELY(!data || !tData || !posList)) return 0;

    int cnt = 0, pos = 0;
    int byteStepLen = CMN_PAD_SIZE(tDataLen*sizeof(uint16_t) + sizeof(uint16_t));
    int intStepLen = byteStepLen / sizeof(int);
    int cmpLen = dataLen - byteStepLen;

    const int* srcPtr = (const int*)data;
    const int* tarPtr = (const int*)tData;
    while (pos < cmpLen) {
        if (tDataLen == *srcPtr++) {// compare length
            if (*tarPtr == *srcPtr &&// compare head
                utils_cmp_reverse_16((const uint16_t*)srcPtr, tData, tDataLen)// compare body
                ) {
                posList[cnt] = pos;
                if (maxPosCnt <= ++cnt) break;

                pos += byteStepLen;
                srcPtr += intStepLen;
            } else {
                ++ srcPtr;
                pos += sizeof(int);
            }
        }
        pos += sizeof(int);
    }
    return cnt;
}

char* utils_cat_strings(int cnt, const char* arg, ...) {
    if (__UNLIKELY(cnt <= 0 || !arg)) return NULL;

    size_t size = 0;
    if (arg) {
        size += strlen(arg);
    }
    const char* tmp = NULL;

    va_list arg_ptr;
    va_start(arg_ptr, arg);
    for (int i=1; i<cnt; ++i) {
        tmp = va_arg(arg_ptr, const char*);
        if (__LIKELY(tmp)) {
            size += strlen(tmp);
        }
    }
    va_end(arg_ptr);

    char* str = (char*)malloc(size + 1);
    if (__LIKELY(str)) {
        char* ptr = str;
        if (__LIKELY(arg)) {
            while ('\0' != (*ptr = *arg ++)) ++ ptr;
        }

        va_start(arg_ptr, arg);
        for (int i=1; i<cnt; ++i) {
            tmp = va_arg(arg_ptr, const char*);
            if (__LIKELY(tmp)) {
                while ('\0' != (*ptr = *tmp ++)) ++ ptr;
            }
        }
        va_end(arg_ptr);
    }

    return str;
}

int utils_hash(const char* name) {
    if (__LIKELY(name)) {
        return utils_hash(name, strlen(name));
    }
    return 0;
}

int utils_hash(const void* data, size_t dataSize) {
    if (__UNLIKELY(!data || dataSize <= 0)) return 0;
    int h = 0, g;
    const char* ptr = (const char*)data;
    while (0 < dataSize --) {
        h = (h << 4) + *ptr++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}

int utils_hash_java(const char* name) {
    if (__LIKELY(name)) {
        return utils_hash_java(name, strlen(name));
    }
    return 0;
}

int utils_hash_java(const void* data, size_t dataSize) {
    if (__UNLIKELY(!data || dataSize <= 0)) return 0;
    int h = 0;
    const char* ptr = (const char*)data;
    while (0 < dataSize --) {
        h = 31 * h + *ptr;
        ++ ptr;
    }
    return h;
}

long long utils_hash_l(const char* name) {
    if (__UNLIKELY(!name)) return 0;
    long long h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}

long long utils_hash_java_l(const char* name) {
    if (__UNLIKELY(!name)) return 0;
    long long h = 0;
    while (*name) {
        h = 31 * h + *name;
        ++ name;
    }
    return h;
}

int utils_hash16(const uint16_t* token, int tokenLen) {
    if (__UNLIKELY(!token || tokenLen <= 0)) return 0;
    unsigned int h = 0, g;
    while (0 < tokenLen--) {
        h = (h << 4) + *token++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}

bool utils_ends_with(const char* s, const char* d) {
    if (__UNLIKELY(!s || !d)) return false;
    return utils_ends_with(s, strlen(s), d, strlen(d));
}

bool utils_ends_with(const char* s, size_t sLen, const char* d, size_t dLen) {
    if (__UNLIKELY(!s || !d || sLen < dLen)) return false;
    return utils_cmp_reverse_8((const uint8_t*)s + (sLen - dLen), (const uint8_t*)d, dLen);
}

bool utils_ends_with_ignore_case(const char* s, const char* d) {
    if (__UNLIKELY(!s || !d)) return false;
    return utils_ends_with_ignore_case(s, strlen(s), d, strlen(d));
}

bool utils_ends_with_ignore_case(const char* s, size_t sLen, const char* d, size_t dLen) {
    if (__UNLIKELY(!s || !d || sLen < dLen)) return false;
    return utils_cmp_reverse_8_ignore_case((const uint8_t*)s + (sLen - dLen), (const uint8_t*)d, dLen);
}

bool utils_starts_with(const char* s, const char* d) {
    if (__LIKELY(s && d)) {
        while (*d && *s ++ == *d) ++d;
        return '\0' == *d;
    }
    return false;
}

bool utils_starts_with(const char* s, size_t sLen, const char* d, size_t dLen) {
    if (__UNLIKELY(!s || !d || sLen < dLen)) return false;
    return utils_cmp_reverse_8((const uint8_t*)s, (const uint8_t*)d, dLen);
}

bool utils_string_equals(const char* s1, const char* s2) {
    if (NULL == s1 || NULL == s2) return false;
    if (s1 == s2) return true;
    return 0 == strcmp(s1, s2);
}

typedef int (*cmnSysPropGetFuncPtr)(const char *, char *);
static cmnSysPropGetFuncPtr g_getProptfuncPtr = NULL;
int utils_property_get(const char *name, char *value) {
    __ASSERT(name && value)
    if (__UNLIKELY(!g_getProptfuncPtr)) {
        void* handle = dlopen("libc.so", RTLD_LAZY);
        __ASSERT(handle)
        if (__LIKELY(handle)) {
            g_getProptfuncPtr = (cmnSysPropGetFuncPtr) dlsym(handle, "__system_property_get");
            __ASSERT(g_getProptfuncPtr)
        }
        // TODO ignore dlclose(handle);
        if (__UNLIKELY(!g_getProptfuncPtr)) return -1;
    }
    return g_getProptfuncPtr(name, value);
}

void utils_string_insert_char(char* str, int pos, char ch) {
    if (__UNLIKELY(!str)) return ;
    
    int i = 0;
    while (*str) {
        if (pos <= i) {
            ch ^= *str;
            *str ^= ch;
            ch ^= *str;
        }
        
        ++ i;
        ++ str;
    }
    if (pos <= i) {
        *str ++ = ch;
        *str = '\0';
    }
}

void utils_reverse_string(const char * src, char * dst) {
    int len = strlen(src);
    int i = 0;
    while (i < len)
    {
        dst[i] = src[len - i -1];
        ++i;
    }
    dst[len] = '\0';
}

