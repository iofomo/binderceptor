package com.ifma.cmpt.demo.main;

import android.app.Service;
import com.ifma.cmpt.testin.env.TstLogger;
import android.content.Intent;
import android.os.IBinder;
import androidx.annotation.Nullable;

public class MainService extends Service {
    private static final String TAG = MainService.class.getSimpleName();

    public void onCreate() {
        super.onCreate();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        TstLogger.bundle(TAG, intent.getExtras());
        new Thread(new Runnable() {
            @Override
            public void run() {
            }
        }).start();
        return START_NOT_STICKY;
    }
}
