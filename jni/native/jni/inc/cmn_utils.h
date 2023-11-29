#ifndef __CMN_UTILS_H__
#define __CMN_UTILS_H__

#include <dirent.h>
#include <fcntl.h>
#include "cmn_macro.h"

#define CMN_PAD_SIZE(s) (((s)+3)&~3)// from sys_binder.cpp
#define __UTILS_IS_PRINT(v) (' '<=(v) && (v)<='~')
#define __StringEqual(p, q) (strlen(p)==strlen(q)&&0==strcmp(p, q))
#define __StringUnequal(p, q) (strlen(p)!=strlen(q)||0!=strcmp(p, q))

void utils_logger(const char* tag, const char* fmt, ...);

/**
 * @brief: dump data
 * */
void utils_hexdump(const void *_data, unsigned size);

bool utils_memcmp_16(const uint16_t *buffer1,const uint16_t *buffer2, size_t count);
bool utils_cmp(const uint8_t *buffer1, const uint16_t *buffer2, size_t count);
bool utils_cmp_reverse(const uint8_t *buffer1,const uint16_t *buffer2, size_t count);
bool utils_cmp_reverse_8(const uint8_t *buffer1, const uint8_t *buffer2, size_t count);
bool utils_cmp_reverse_8_ignore_case(const uint8_t* buffer1, const uint8_t* buffer2, size_t count);
bool utils_cmp_reverse_16(const uint16_t *buffer1,const uint16_t *buffer2, size_t count);

void utils_print_string16(const char* tag, const void* data, size_t len);

/**
 * @brief: dlopen the function from lib
 * */
typedef struct {
    const char* funName;
    void** funPtr;
} cmnDLSYM_API_FUN;
void* utils_dlsym(const char* libName, const char* funcName);
void* utils_dlsym_global(const char* libName, const char* funcName);
bool  utils_dlsyms(const char* libName, cmnDLSYM_API_FUN* funcs, int length);

/**
 * @brief: test all the function name from names
 * */
void* utils_dlsym_ex(const char* libName, const char** names, int nameLen);

/**
 * @brief: get string16 length
 * */
size_t utils_str16len(const uint16_t *str);

char* utils_malloc_string(const char* _data, int _pos, int len);
size_t utils_set_string(char*& desVal, const char* val);

/**
 * @brief: string utils
 * */
const char* utils_strnstr(const char *src,  size_t srcLen, const char *substr);
char* utils_string_sub(const char *buf, const char *head, const char *end);
int utils_string_strip(char* str);
int utils_string_strip(char* str, char ch);
int utils_string_lstrip(char* str);
int utils_string_lstrip(char* str, char ch);
int utils_string_rstrip(char* str, size_t len);
int utils_string_rstrip(char* str, size_t len, char ch);

void utils_string_lower(char* s);
void utils_string_upper(char* s);

/**
 * @brief: split string into items with target char
 * @param:
 *      src: source string
 *       ch: target char
 *    items: out split items
 * itemsLen: out split items length
 * @return:
 *      count of split items
 * */
int utils_string_split(char* src, char ch, char* items[], int itemsLen);

/**
 * @brief: replace old char to new char
 * @param:
 *      str: source string
 *      src: old char
 *      des: new char
 * @return:
 *      count of replaced
 * */
int utils_string_replace(char* str, char src, char des);

/**
 * @brief: simple convert string16 to string6
 *         just for print letters
 * */
char* utils_convert_char16(const uint16_t* _data16, int _dataLen);
void  utils_convert_char16(const uint16_t* _data16, char* _data8/*OUT*/, int _dataLen);
uint16_t* utils_char_to_uint16(const char* data);

/**
 * @brief: search all the position from data
 * @param:
 *  data: source data
 *  dataLen: source data length
 *  tData: target data
 *  tDataLen: target data length by uint16
 *  posList: out results, position list
 *  maxPosCnt: max position list count
 * @return:
 *  0: not found, otherwise found count
 * */
int utils_search_string16(const uint8_t* data, int dataLen, const uint16_t* tData, int tDataLen, int* posList/*out*/, int maxPosCnt);

/**
 * @brief: cat strings, the last argument must be NULL
 * */
char* utils_cat_strings(int cnt, const char* arg, ...);

/**
 * @brief: cacl hash value for char string and String16
 *          used for token string
 *          do not used unsigned int, Java without unsigned type !!!
 * */
int utils_hash(const char* name);
int utils_hash(const void* data, size_t dataSize);
int utils_hash_java(const char* name);
int utils_hash_java(const void* data, size_t dataSize);
int utils_hash16(const uint16_t* token, int tokenLen);

/**
 * @brief: start/end with target string from source string
 * */
bool utils_ends_with(const char* s, const char* d);
bool utils_ends_with(const char* s, size_t sLen, const char* d, size_t dLen);
bool utils_ends_with_ignore_case(const char* s, const char* d);
bool utils_ends_with_ignore_case(const char* s, size_t sLen, const char* d, size_t dLen);
bool utils_starts_with(const char* s, const char* d);
bool utils_starts_with(const char* s, size_t sLen, const char* d, size_t dLen);

bool utils_string_equals(const char* s1, const char* s2);

/**
 * @brief: get property info feom system
 * */
int utils_property_get(const char *name, char *value);

long long utils_hash_l(const char* name);
long long utils_hash_java_l(const char* name);

#endif // end of __CMN_UTILS_H__
