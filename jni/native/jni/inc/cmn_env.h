#ifndef __CMN_ENV_H__
#define __CMN_ENV_H__

//#define ENV_SDK_INT_GINGERBREAD             9   //November 2010: Android 2.3.
//#define ENV_SDK_INT_GINGERBREAD_MR1         10  //February 2011: Android 2.3.3.
//#define ENV_SDK_INT_HONEYCOMB               11  //February 2011: Android 3.0.
//#define ENV_SDK_INT_HONEYCOMB_MR1           12  //May 2011: Android 3.1.
//#define ENV_SDK_INT_HONEYCOMB_MR2           13  //June 2011: Android 3.2.
//#define ENV_SDK_INT_ICE_CREAM_SANDWICH      14  //October 2011: Android 4.0.
//#define ENV_SDK_INT_ICE_CREAM_SANDWICH_MR1  15  //December 2011: Android 4.0.3.
//#define ENV_SDK_INT_JELLY_BEAN              16  //June 2012: Android 4.1.
//#define ENV_SDK_INT_JELLY_BEAN_MR1          17  //Moar jelly beans: Android 4.2
//#define ENV_SDK_INT_JELLY_BEAN_MR2          18  //Jelly Bean MR2: Android 4.3
//#define ENV_SDK_INT_KITKAT                  19  //KitKat: Android 4.4.2
//#define ENV_SDK_INT_KITKAT_WATCH            20  //KitKat: Android 4.4.4
//#define ENV_SDK_INT_LOLLIPOP                21  //lollipop: Android 5.0
//#define ENV_SDK_INT_LOLLIPOP_MR1            22  //lollipop: Android 5.1
//#define ENV_SDK_INT_M                       23  //Android 6.0
//#define ENV_SDK_INT_N                       24  //Android 7.0
//#define ENV_SDK_INT_N_MR1                   25  //Android 7.1
#define ENV_SDK_INT_O_8_26                  26  //Android 8.0
#define ENV_SDK_INT_O_MR1_81_27             27  //Android 8.1
#define ENV_SDK_INT_P_9_28                  28  //Android 9.0
#define ENV_SDK_INT_Q_10_29                   29  //Android 10.0
#define ENV_SDK_INT_R_11_30                   30  //Android 11.0
#define ENV_SDK_INT_S_12_31                   31  //Android 12.0
#define ENV_SDK_INT_T_13_32                   32  //Android 13.0
#define ENV_SDK_INT_T_14_33                   33  //Android 14.0

enum {// for EnvUu::brand()
    ENV_BRAND_NORMAL = 0x0,
    ENV_BRAND_XIAOMI,
    ENV_BRAND_HTC,
    ENV_BRAND_MOTO,
    ENV_BRAND_SAMSUNG,
    ENV_BRAND_SONY, // 05
    ENV_BRAND_SEMC,
    ENV_BRAND_ONEPLUS,
    ENV_BRAND_HUAWEI,
    ENV_BRAND_NUBIA,
    ENV_BRAND_ZTE, // 10
    ENV_BRAND_SMARTISAN,
    ENV_BRAND_VIVO,
    ENV_BRAND_OPPO,
    ENV_BRAND_HUAWEI_HMOS,
};

enum {
    ENV_CPU_UNKNOWN = 0,
    ENV_CPU_ARM,
    ENV_CPU_ARM_64,
    ENV_CPU_X86,
    ENV_CPU_X86_64,
    ENV_CPUT_MIPS,
    ENV_CPUT_MIPS_64,
};

class CmnEnv {
public:
    // sdk version, see @@ ENV_SDK_INT_...
    static void setSdkVersion(int v);
    static int sdkVersion();

    // see @@ ENV_Cpu_...
    static int cpuType();

    // see @@ ENV_BRAND_...
    static int brand();
};

#endif// end of __CMN_ENV_H__
