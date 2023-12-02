# 集成开发

### 环境配置

#### 基础环境

-   **Android Studio：**4.2
-   **Gradle：**6.9.2
-   **NDK：**21.4.7075529
-   **Python：**2.7.x/3.x

#### 库依赖

```groovy
dependencies {
    implementation (name: 'cmpt-mts-binderceptor', ext: 'aar')
}
```

### 日志打印

提供了三个不同级别的`Binder`日志输出，级别可叠加，由于应用进程`Binder`通信机器频繁，因此建议通常只需开启`SIMPLE`级别即可。

接口定义如下：

```java
package android.app.ifma.mts.binderceptor;

public class BinderceptorManager {
    // 首先需要初始化
    public static void init() {
        callNative(INIT, 0, null);
    }

    public static int EBinderceptorDemoFlag_Print_Simple           = 0x1;
    public static int EBinderceptorDemoFlag_Print_Transaction_Data = 0x2;
    public static int EBinderceptorDemoFlag_Print_Write_Read       = 0x4;
  
    // 默认不输出日志，可以设置日志输出级别
    public static void setLogger(int flag) {
        callNative(SET_LOGGER, flag, null);
    }
}
```

#### Simple日志

输出内容如下：（包含`Binder`服务名称和方法`Code`值）

```tex
Binder::tgt: 0x9e77682, 10002: com.huawei.iaware.sdk.ISDKManager
Binder::tgt: 0x3887b52, 147: android.content.pm.IPackageManager
Binder::tgt: 0x76c2822, 56: com.huawei.android.view.IHwWindowManager
Binder::tgt: 0xdc3c612, 41: com.huawei.android.app.IHwActivityTaskManager
Binder::tgt: 0x367556e, 2: android.view.IWindowSession
Binder::tgt: 0x255ce27, 3: android.magicwin.IHwMagicWindow
Binder::tgt: 0x76c2822, 56: com.huawei.android.view.IHwWindowManager
Binder::tgt: 0x984dc75, 24: android.content.pm.IShortcutService
Binder::tgt: 0x9a978a2, 21: android.content.IContentProvider
Binder::tgt: 0x19c1b12, 10: android.aps.IHwApsManager
```

#### Transaction_Data日志：

输出内容如下：（包含`txn`完整数据结构内容，特别是参数内容）

```tex
Trace   : target:       1   cookie:       0   code:      23   flags:   0x12(READ REPLY)
Trace   :    pid:       0      uid:       0   size:     196    offs:8
Trace   : 00000000:  00 00 00 80 ff ff ff ff  54 53 59 53 1c 00 00 00  ........TSYS....
Trace   : 00000010:  61 00 6e 00 64 00 72 00  6f 00 69 00 64 00 2e 00  a.n.d.r.o.i.d...
Trace   : 00000020:  61 00 70 00 70 00 2e 00  49 00 41 00 63 00 74 00  a.p.p...I.A.c.t.
Trace   : 00000030:  69 00 76 00 69 00 74 00  79 00 4d 00 61 00 6e 00  i.v.i.t.y.M.a.n.
Trace   : 00000040:  61 00 67 00 65 00 72 00  00 00 00 00 85 2a 62 73  a.g.e.r......*bs
Trace   : 00000050:  13 01 00 00 00 38 dd 2a  71 00 00 b4 00 05 e9 31  .....8.*q......1
Trace   : 00000060:  71 00 00 b4 01 00 00 0c  1a 00 00 00 63 00 6f 00  q...........c.o.
Trace   : 00000070:  6d 00 2e 00 69 00 66 00  6d 00 61 00 2e 00 74 00  m...i.f.m.a...t.
Trace   : 00000080:  72 00 61 00 6e 00 73 00  65 00 63 00 2e 00 63 00  r.a.n.s.e.c...c.
Trace   : 00000090:  6f 00 6e 00 74 00 61 00  69 00 6e 00 65 00 72 00  o.n.t.a.i.n.e.r.
Trace   : 000000a0:  00 00 00 00 08 00 00 00  73 00 65 00 74 00 74 00  ........s.e.t.t.
Trace   : 000000b0:  69 00 6e 00 67 00 73 00  00 00 00 00 00 00 00 00  i.n.g.s.........
Trace   : 000000c0:  01 00 00 00                                       ....
Trace   : binder object offs:0x4c  type:0x73622a85  flags:0x113  ptr:0x2add3800  cookie:0x31e90500
```

#### Write_Read日志：

输出内容如下：（包含第一层`Binder`通信命令内容）

```
write_buffer:0xb400007107d1d400, write_consumed:68, write_size:68
00000000:  00 63 40 40 14 00 00 00  00 00 00 00 00 00 00 00  .c@@............
00000010:  00 00 00 00 01 00 00 00  12 00 00 00 00 00 00 00  ................
00000020:  00 00 00 00 54 00 00 00  00 00 00 00 00 00 00 00  ....T...........
00000030:  00 00 00 00 00 4d 3a ac  70 00 00 b4 00 00 00 00  .....M:.p.......
00000040:  00 00 00 00                                       ....
BR_NOOP: 0x720c
BR_TRANSACTION_COMPLETE: 0x7206
BR_REPLY: 0
```

### 参数过滤

由于`Binder`通信日志繁多，无法甄别，因此我们需要过滤接口，比如只需要打印那些方法的参数中包含目标内容。如只打印那些`Binder`通信中包含字符串"com.demo"（`Binder`通信字符通常为`uint16_t`宽字符）内容的调用日志。

由于Binder通信调用分为同步（`target`）和异步（`oneway`）两种方式，因此需要在对应的两个方法回调中处理，如：

```c++
// target argument：com.demo
static uint16_t g_debug_target[] = {'c','o','m','.','d','e','m','o'};
static const uint32_t g_debug_target_length = 8;

static bool doCheckService(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token, bool oneway) {
    if ((txn->data_size < sizeof(g_debug_target) + sizeof(int32_t))) return false;

    const uint8_t *ptr = (const uint8_t *) txn->data.ptr.buffer;
    for (int i=0, len = txn->data_size-sizeof(g_debug_target)-sizeof(int32_t); i <= len; ++i) {
        uint32_t* intPtr = (uint32_t*)(ptr + i);
        if (*intPtr != g_debug_target_length) continue;// fast length check
        if (memcmp(ptr + i + sizeof(int32_t), g_debug_target, sizeof(g_debug_target)) != 0) continue;

        // match then print
        token.print(txn, oneway);
    }

    return true;
}

bool BinderceptorDebug::onFrameworkBinderTarget(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;
    return doCheckService(txn, pInfo, token, false);
}

bool BinderceptorDebug::onFrameworkBinderOneway(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;
    return doCheckService(txn, pInfo, token, true);
}
```

### 数据修改

有些时候我们需要修改通信的参数，这里我们以当应用查找对应应用（`com.demo`）是否存在时，我们始终返回不存在为例，如：

>这里以应用程序调用如下方法为例：
>
>PackageInfo getPackageInfo(String packageName, int flags, int userId);

```c++
// target server: android.content.pm.IPackageManager
static uint16_t g_debug_server[] = {'a','n','d','r','o','i','d','.','c','o','n','t','e','n','t','.','p','m','.','I','P','a','c','k','a','g','e','M','a','n','a','g','e','r'};
static const uint32_t g_debug_server_length = 34;

// target argument：com.demo
static uint16_t g_debug_target[] = {'c','o','m','.','d','e','m','o'};
static const uint32_t g_debug_target_length = 8;

static bool pms_get_package_info(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token, bool oneway) {
    BinderParcel* pr = __PARCEL_NEW_DATA((const uint8_t*)(txn->data.ptr.buffer), txn->data_size);
    if (__UNLIKELY(!pr)) return false;
    __PARCEL_AUTO_FREE(pr);

    if (0 != pr->skipToken()) return false;// skip strict model and token

    size_t len = 0;
    const char16_t* pkg16 = pr->readString16Inplace(&len);
    if (g_debug_target_length != len || !pkg16) return 0;// maybe package name = ""

    if (memcmp(pkg16, g_debug_target, sizeof(g_debug_target)) == 0) {
        // changed the package arguments
        ((char16_t*)pkg16)[len-1] = ' ';// fix, make sure throw name not found exception
        return true;
    }
    return false;
}

/**
 *   如何找到对应的方法Code：
 *   1. FindClass("android/content/pm/IPackageManager$Stub");
 *   2. 获取静态成员的值："TRANSACTION_getPackageInfo"
 */
bool BinderceptorDebug::onFrameworkBinderTarget(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;
    if (txn->code == 10) {
    	   return doCheckService(txn, pInfo, token, false);
    }
    return false;
}

bool BinderceptorDebug::onFrameworkBinderOneway(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;
    if (txn->code == 10 && token.isTargetToken(g_debug_server, g_debug_server_length)) {
    	   return doCheckService(txn, pInfo, token, true);
    }
    return false;
}
```

### 调用拦截

如果需要直接拒绝调用此函数，则需要告诉Binder拦截框架，然后处理拦截后返回的数据。

>这里以应用程序调用如下方法为例：
>
>ParcelFileDescriptor openDevice(String deviceName, String packageName);

```c++
#define BINDER_TARGET_SERVER				  1// do not changed
#define BINDER_TARGET_SERVER_OPEN_DEVICE		1

// target server: android.hardware.usb.IUsbManager
static uint16_t g_debug_server[] = {'a','n','d','r','o','i','d','.''h','a','r','d','w','a','r','e','.','u','s','b','.','I','U','s','b','M','a','n','a','g','e','r'};
static const uint32_t g_debug_server_length = 32;

/**
 *   如何找到对应的方法Code：
 *   1. FindClass("android/hardware/usb/IUsbManager$Stub");
 *   2. 获取静态成员的值："TRANSACTION_OpenDdevice"
 */
bool BinderceptorDebug::onFrameworkBinderTarget(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;
    if (txn->code == 10) {
    	   return doCheckService(txn, pInfo, token, false);
    }
    return false;
}

bool BinderceptorDebug::onFrameworkBinderOneway(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token) {
    if (token.isInvalid()) return false;
  
    if (txn->code == 10 && token.isTargetToken(g_debug_server, g_debug_server_length)) {
         pInfo.type = BINDER_TARGET_SERVER;
         pInfo.method = BINDER_TARGET_SERVER_OPEN_DEVICE;
         pInfo.addAction(EIoctlAction_After_Faker);
    	   return true;
    }
    return false;
}

void BinderceptorDebug::onFrameworkBinderAfterFaker(BinderParcel& reply, TBinderInfo& pInfo) {
    // 在这里修复返回的结果，以保证上层调用不出错
    if (BINDER_TARGET_SERVER_OPEN_DEVICE == pInfo.method) {
        reply.writeInt32(0);// no Exception
        reply.writeInt32(0);// no object
    }
}
```

