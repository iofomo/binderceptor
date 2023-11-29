package com.ifma.cmpt.demo.main;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.os.Bundle;
import android.text.TextUtils;
import android.text.method.ScrollingMovementMethod;
import android.widget.ScrollView;
import android.widget.TextView;

import com.ifma.cmpt.testin.env.TstConsts;
import com.ifma.cmpt.testin.env.TstHandler;
import com.ifma.cmpt.testin.module.TstRunner;
import com.ifma.cmpt.demo.binderceptor.R;

public class ConsoleActivity extends Activity {
    private TextView mTextConsole;
    private ScrollView mScrollView;
    protected BroadcastReceiver mBR;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_console);
        initView();
        initData();
        initTest();
    }

    protected void initView() {
        mTextConsole = (TextView) findViewById(R.id.console);
        mScrollView = (ScrollView) findViewById(R.id.scrollView);
        mTextConsole.setMovementMethod(ScrollingMovementMethod.getInstance());
    }

    protected void initData() {
        addMessage("Console started ...");
        final String from = getIntent().getStringExtra(TstConsts.KEY_FROM);
        if (!TextUtils.isEmpty(from)) {
            addMessage("From " + from);
        }
    }

    protected void initTest() {
        final Bundle args = getIntent().getExtras();
//        if (!TextUtils.equals(TstConsts.VALUE_UNITTEST, args.getString(TstConsts.KEY_TYPE))) return;

        TstRunner.runTest(args, new TstRunner.ITstPrinter() {
            @Override
            public void onTstStart() {

            }

            @Override
            public void onTstDone() {
            }

            @Override
            public void onTstPrint(String s) {
                addMessage(s);
            }
        });
    }

    protected void addMessage(final String msg) {
        if (TstHandler.isMainThread()) {
            doPrint(msg);
        } else {
            TstHandler.postUi(new Runnable() {
                @Override
                public void run() {
                    doPrint(msg);
                }
            });
        }
    }

    private void doPrint(String msg) {
        try {
            mTextConsole.append(null != msg ? msg : "null");
            mTextConsole.append("\n");
            int offset = mTextConsole.getMeasuredHeight() - mScrollView.getMeasuredHeight();
            if (offset < 0) {
                offset = 0;
            }
            mScrollView.scrollTo(0, offset);
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    protected void onDestroy() {
        if (null != mBR) {
            try {
                unregisterReceiver(mBR);
            } catch (Throwable e) {
                e.printStackTrace();
            } finally {
                mBR = null;
            }
        }
        TstRunner.cancel();
        super.onDestroy();
    }
}
