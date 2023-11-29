package com.ifma.cmpt.demo.main;

import com.ifma.cmpt.testin.env.TstConsts;
import com.ifma.cmpt.testin.env.TstLogger;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;


public class MainReceiver extends BroadcastReceiver {
    private static final String TAG = MainReceiver.class.getSimpleName();

    public static void register(Context ctx) {
        ctx.registerReceiver(new MainReceiver(), new IntentFilter(TstConsts.getActionReceiver()));
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        TstLogger.bundle(TAG, intent.getExtras());
    }
}
