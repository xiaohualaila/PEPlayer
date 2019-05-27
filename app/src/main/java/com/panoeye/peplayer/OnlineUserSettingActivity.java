package com.panoeye.peplayer;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.ref.WeakReference;

/**
 * Created by tir on 2016/10/5.
 */
public class OnlineUserSettingActivity extends AppCompatActivity {
    private static final String TAG = "OnlineUserSetting";

    final WeakReference<OnlineUserSettingActivity> self = new WeakReference<OnlineUserSettingActivity>(this);
    TextView usernameField, passwordField;
    String ip;
    int port;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_onlineusersetting);
        usernameField = findViewById(R.id.usernameField);
        passwordField = findViewById(R.id.passwordField);
        ip = (String)getIntent().getSerializableExtra("IP");
        port = (int)getIntent().getSerializableExtra("PORT");
        SharedPreferences sPerfs= PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        usernameField.setText(sPerfs.getString("USERNAME", ""));
        passwordField.setText(sPerfs.getString("PASSWORD", ""));

//        usernameField.setText("admin");
//        passwordField.setText("888888");
//        next(null);



    }

    public void back(View btn) {
        this.finish();
    }


    public void next(View btn) {
        Intent intent = new Intent(self.get(), LoadingActivity.class);
        startActivity(intent);
        Thread subThread = new Thread(new Runnable() {
            @Override
            public void run() {
//                int c = 0;
//                Log.d(TAG, "run: "+LoadingActivity.loadingActivity);
                while (LoadingActivity.loadingActivity == null){
//                    c++;
//                    Log.d(TAG, "run~线程等待次数："+c);
                    try {
                        Thread.sleep(10);//等待10ms
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                String username = usernameField.getText().toString();
                String password = passwordField.getText().toString();
                if (JNIServerLib.registOnlinePano(ip, port, username, password) == true) {
                    Log.i("sss","ip  "+ip);
                    Log.i("sss","port  "+port);
                    Intent intent = new Intent(self.get(), OnlineListActivity.class);
                    SharedPreferences sPerfs= PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
                    SharedPreferences.Editor editor = sPerfs.edit();
                    editor.putString("USERNAME", username);
                    editor.putString("PASSWORD", password);
                    editor.commit();
                Log.d(TAG, "run: ~~~");
                    mHandler.sendEmptyMessage(1);
                    startActivity(intent);
                } else {
                    mHandler.sendEmptyMessage(2);
                }
            }
        });
        subThread.start();
//        Log.d(TAG, "next: "+Thread.currentThread().getName());
//        Log.d(TAG, "next: "+Thread.currentThread().getPriority());
//        Log.d(TAG, "next~: "+subThread.getName());
//        Log.d(TAG, "next~: "+subThread.getId());
//        Log.d(TAG, "next~: "+subThread.getState());
//        Log.d(TAG, "next~: "+subThread.getPriority());
//        Log.d(TAG, "next~: "+subThread.isAlive());
    }
    final private Handler mHandler = new Handler(){
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 0 :
                    break;
                case 1 :
                    LoadingActivity.loadingActivity.close();
                    break;
                case 2:
                    LoadingActivity.loadingActivity.close();
                    new AlertDialog.Builder(self.get())
                            .setTitle("错误")
                            .setMessage(JNIServerLib.getLastErrorMessage())
                            .setPositiveButton("确定", null)
                            .setCancelable(false)
                            .show();
                default :
                    break;
            }
        }
    };
}
