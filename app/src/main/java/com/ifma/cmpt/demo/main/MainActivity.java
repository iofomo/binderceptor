package com.ifma.cmpt.demo.main;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.ifma.cmpt.testin.env.TstConsts;
import com.ifma.cmpt.testin.env.TstHandler;
import com.ifma.cmpt.testin.env.TstLogger;
import com.ifma.cmpt.testin.module.TstPermissionGrantor;
import com.ifma.cmpt.demo.binderceptor.BuildConfig;
import com.ifma.cmpt.demo.binderceptor.R;


public class MainActivity extends Activity implements View.OnClickListener {
    private static final String TAG = "MainActivity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
        TstPermissionGrantor.grant(this);
        initData();
    }

    private void initView() {
        TextView text = (TextView) findViewById(R.id.main_version);
        text.setText("v" + BuildConfig.VERSION_NAME + "(" + BuildConfig.VERSION_CODE + ")");
        View v;
        v = findViewById(R.id.item_unittest);
//        v.setVisibility(View.GONE);
        v.setOnClickListener(this);

        v = findViewById(R.id.item_xxx);
//        v.setVisibility(View.GONE);
        v.setOnClickListener(this);
    }

    public void onClick(View v) {
        switch (v.getId()) {
        case R.id.item_unittest:
        {
            Intent i = new Intent(this, ConsoleActivity.class);
            i.putExtra(TstConsts.KEY_FROM, "MainActivity");
            i.putExtra(TstConsts.KEY_TYPE, TstConsts.VALUE_UNITTEST);
            startActivity(i);
        }   break;
        case R.id.item_xxx:
            break;
        default: break;
        }
    }

    private void initData() {
        final Bundle args = getIntent().getExtras();
        TstLogger.bundle(TAG, args);
        if (null == args) return;

        TstHandler.post(new Runnable() {
            @Override
            public void run() {
                final int type = args.getInt(TstConsts.KEY_TYPE);
                switch (type) {
                case TstConsts.TYPE_CALL_UNIT_TEST:

                    break;
                default: break;
                }
            }
        });
    }

}
