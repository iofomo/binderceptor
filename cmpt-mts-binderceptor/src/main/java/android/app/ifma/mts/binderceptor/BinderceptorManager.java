package android.app.ifma.mts.binderceptor;

public class BinderceptorManager {
    private static final String TAG = "BinderceptorManager";

    static {
        try {
            System.loadLibrary("ifmabinderceptor-core");
            System.loadLibrary("ifmabinderceptor-jni");
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public static void setLogger(int flag) {
        callNative(SET_LOGGER, flag, null);
    }

    public static void init() {
        callNative(INIT, 0, null);
    }

    public static int EBinderceptorDemoFlag_Print_Simple           = 0x1;
    public static int EBinderceptorDemoFlag_Print_Transaction_Data = 0x2;
    public static int EBinderceptorDemoFlag_Print_Write_Read       = 0x4;
//    EBinderceptorDemoFlag_Filter                 = 0x8;

    private static final int INIT           =1;
    private static final int SET_LOGGER     =2;

    private static native Object callNative(int typ, int argInt, Object args);
}
