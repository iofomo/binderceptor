package com.ifma.cmpt.demo;

import android.app.Application;
import android.content.Context;

import com.ifma.cmpt.testin.env.TstEnv;
import com.ifma.cmpt.utils.CmnInitor;

public class DemoApplication extends Application {
    private static final String TAG = "DemoApplication";

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        TstEnv.init(this, CmnInitor.CMN_INITOR_MODE_ENTRANCE);
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

}
