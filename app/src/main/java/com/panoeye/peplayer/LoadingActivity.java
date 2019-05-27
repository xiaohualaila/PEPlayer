package com.panoeye.peplayer;

import android.content.res.TypedArray;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ProgressBar;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by tir on 2016/9/5.
 */
public class LoadingActivity extends AppCompatActivity {
    private static final String TAG = "LoadingActivity";
    final private Handler mHandler = new Handler(){
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 0:
                    int pos = JNIServerLib.getOnlinePanoFileDownloadingPos();
                    bar.setProgress(pos);
                    break;
                default :
                    break;
            }
        }
    };
    static LoadingActivity loadingActivity;
    ProgressBar bar;
    Timer posTimer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
//        Log.d(TAG, "onCreate: ~~~");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_loading);
        bar = findViewById(R.id.progressBar);
        bar.setVisibility(View.GONE);
        loadingActivity = this;
    }

    public void startPos() {
        bar.setMax(100);
        bar.setProgress(0);
        bar.setVisibility(View.VISIBLE);
        posTimer = new Timer();
        TimerTask task = new TimerTask(){
            public void run() {
                mHandler.sendEmptyMessage(0);
            }
        };
        posTimer.schedule(task, 0, 50);
    }

    public void close() {
        this.finish();
        overridePendingTransition(0, R.anim.abc_fade_out);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return false;
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy: ~~~");
        super.onDestroy();
        loadingActivity = null;
    }
}
