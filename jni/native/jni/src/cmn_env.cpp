#include <stdlib.h>
#include <stdint.h>
#include "cmn_env.h"
#include "cmn_utils.h"

// ---------------------------------------------------------------------------------------------------------
static int g_sdk_version = 0;
void CmnEnv::setSdkVersion(int v) { g_sdk_version = v; }

int CmnEnv::sdkVersion() {
    if (__LIKELY(0 < g_sdk_version)) return g_sdk_version;

    char value[PROPERTY_VALUE_MAX] = {0};
    utils_property_get("ro.build.version.sdk", value);

    char* end = NULL;
    long v = strtol(value, &end, 10);
    if (end && *end == '\0' && v == (int)v) {
        g_sdk_version = (int)v;
    } else {
        __ASSERT(false);
        g_sdk_version = ENV_SDK_INT_Q_10_29;// default 10.0
    }

    return g_sdk_version;
}

int CmnEnv::cpuType() {
#if defined(__arm__)
    return ENV_CPU_ARM;
#elif defined(__i386__)
    return ENV_CPU_X86;
#elif defined(__mips64)
    return ENV_CPUT_MIPS_64;
#elif defined(__mips__)
    return ENV_CPUT_MIPS;
#elif defined(__aarch64__)
    return ENV_CPU_ARM_64;
#elif defined(__x86_64__)
    return ENV_CPU_X86_64;
#else
    return ENV_CPU_UNKNOWN;
#endif
}

#define _DEF_TOUPPER(p) (('a'<=(p)&&(p)<='z') ? 'A'+((p)-'a') : (p))
#define __DEF_PROPERTY_BRAND_CMP(a, b, c, d)         c <= a && utils_cmp_reverse_8((const uint8_t*)(b), (const uint8_t*)(d), c)
#define __PROPERTY_BRAND_CMP_HTC(len, val)           __DEF_PROPERTY_BRAND_CMP(len, val, 3, "HTC")
#define __PROPERTY_BRAND_CMP_MOTO(len, val)          __DEF_PROPERTY_BRAND_CMP(len, val, 4, "MOTO")
#define __PROPERTY_BRAND_CMP_XIAOMI(len, val)        __DEF_PROPERTY_BRAND_CMP(len, val, 6, "XIAOMI")
#define __PROPERTY_BRAND_CMP_SAMSUNG(len, val)       __DEF_PROPERTY_BRAND_CMP(len, val, 7, "SAMSUNG")
#define __PROPERTY_BRAND_CMP_SONY(len, val)          __DEF_PROPERTY_BRAND_CMP(len, val, 4, "SONY")
#define __PROPERTY_BRAND_CMP_SEMC(len, val)          __DEF_PROPERTY_BRAND_CMP(len, val, 4, "SEMC")
#define __PROPERTY_BRAND_CMP_ONEPLUS(len, val)       __DEF_PROPERTY_BRAND_CMP(len, val, 7, "ONEPLUS")
#define __PROPERTY_BRAND_CMP_HUAWEI(len, val)        __DEF_PROPERTY_BRAND_CMP(len, val, 6, "HUAWEI")
#define __PROPERTY_BRAND_CMP_NUBIA(len, val)         __DEF_PROPERTY_BRAND_CMP(len, val, 5, "NUBIA")
#define __PROPERTY_BRAND_CMP_ZTE(len, val)           __DEF_PROPERTY_BRAND_CMP(len, val, 3, "ZTE")
#define __PROPERTY_BRAND_CMP_SMARTISAN(len, val)     __DEF_PROPERTY_BRAND_CMP(len, val, 9, "SMARTISAN")
#define __PROPERTY_BRAND_CMP_VIVO(len, val)          __DEF_PROPERTY_BRAND_CMP(len, val, 4, "VIVO")
#define __PROPERTY_BRAND_CMP_OPPO(len, val)          __DEF_PROPERTY_BRAND_CMP(len, val, 4, "OPPO")

static int g_env_brand = -1;
int CmnEnv::brand() {
    if (__LIKELY(0 <= g_env_brand)) return g_env_brand;

    char val[PROPERTY_VALUE_MAX+1] = {0};
    utils_property_get("ro.product.manufacturer", val);

    int len = 0;
    while (0 != val[len]) {
        val[len] = _DEF_TOUPPER(val[len]);
        ++ len;
    }
    LOGD("sp_pb: %s", val)

    do {
        if (__PROPERTY_BRAND_CMP_XIAOMI(len, val)) { g_env_brand = ENV_BRAND_XIAOMI; break; }
        if (__PROPERTY_BRAND_CMP_VIVO(len, val)) { g_env_brand = ENV_BRAND_VIVO; break; }
        if (__PROPERTY_BRAND_CMP_OPPO(len, val)) { g_env_brand = ENV_BRAND_OPPO; break; }
        if (__PROPERTY_BRAND_CMP_HUAWEI(len, val)) {
            utils_property_get("hw_sc.build.os.apiversion", val);
            int api = atoi(val);
            if (api >= 5) {
                g_env_brand = ENV_BRAND_HUAWEI_HMOS;
            } else {
                g_env_brand = ENV_BRAND_HUAWEI;
            }
            break;
        }
        if (__PROPERTY_BRAND_CMP_ONEPLUS(len, val)) { g_env_brand = ENV_BRAND_ONEPLUS; break; }
        if (__PROPERTY_BRAND_CMP_NUBIA(len, val)) { g_env_brand = ENV_BRAND_NUBIA; break; }
        if (__PROPERTY_BRAND_CMP_ZTE(len, val)) { g_env_brand = ENV_BRAND_ZTE; break; }
        if (__PROPERTY_BRAND_CMP_MOTO(len, val)) { g_env_brand = ENV_BRAND_MOTO; break; }
        if (__PROPERTY_BRAND_CMP_SAMSUNG(len, val)) { g_env_brand = ENV_BRAND_SAMSUNG; break; }
        if (__PROPERTY_BRAND_CMP_SONY(len, val)) { g_env_brand = ENV_BRAND_SONY; break; }
        if (__PROPERTY_BRAND_CMP_SEMC(len, val)) { g_env_brand = ENV_BRAND_SEMC; break; }		
        if (__PROPERTY_BRAND_CMP_SMARTISAN(len, val)) { g_env_brand = ENV_BRAND_SMARTISAN; break; }
        if (__PROPERTY_BRAND_CMP_HTC(len, val)) { g_env_brand = ENV_BRAND_HTC; break; }

        g_env_brand = ENV_BRAND_NORMAL;
    } while (false);

//    LOGD("ue_eb: %d", g_env_brand)
    return g_env_brand;
}
