package com.ifma.cmpt.demo.test;

import android.app.ifma.mts.binderceptor.BinderceptorManager;
import android.content.Context;
import android.net.ConnectivityManager;

import com.ifma.cmpt.testin.env.TstLogger;
import com.ifma.cmpt.testin.module.TstCaseBase;

public class CommonCase extends TstCaseBase {
    private static final String TAG = "CommonCase";

    protected void setUp() throws Exception {
        TstLogger.d(TAG, "setUp");
        super.setUp();
        BinderceptorManager.init();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        TstLogger.d(TAG, "tearDown");
    }

    public void doBinderCall() {
        ConnectivityManager cm = (ConnectivityManager)getContext().getSystemService(Context.CONNECTIVITY_SERVICE);
        try {
            cm.getActiveNetwork();
        } catch (Throwable e) {
            e.printStackTrace();
        }
        getContext().getPackageManager().getNameForUid(android.os.Process.myUid());
    }

    public void testExample() throws Exception {
        // test 1
        TstLogger.d(TAG, "setLogger Print_Simple");
        BinderceptorManager.setLogger(BinderceptorManager.EBinderceptorDemoFlag_Print_Simple);
        doBinderCall();

        // test 2
        TstLogger.d(TAG, "setLogger Print_Transaction_Data");
        BinderceptorManager.setLogger(BinderceptorManager.EBinderceptorDemoFlag_Print_Transaction_Data);
        doBinderCall();

        // test 3
        TstLogger.d(TAG, "setLogger Print_Write_Read");
        BinderceptorManager.setLogger(BinderceptorManager.EBinderceptorDemoFlag_Print_Write_Read);
        doBinderCall();

        // test 4
        TstLogger.d(TAG, "setLogger Print_Simple");
        BinderceptorManager.setLogger(BinderceptorManager.EBinderceptorDemoFlag_Print_Simple);
        doBinderCall();
    }

}
