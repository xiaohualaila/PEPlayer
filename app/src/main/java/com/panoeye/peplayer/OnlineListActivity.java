package com.panoeye.peplayer;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by tir on 2016/10/9.
 */
public class OnlineListActivity extends AppCompatActivity {
    private static final String TAG = "OnlineListActivity";

    final WeakReference<OnlineListActivity> self = new WeakReference<>(this);
    private ArrayList<String> fileList = new ArrayList<>();
    private ListView listView;
    ArrayAdapter adapter;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate: ~~~");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_onlinelist);
        listView = findViewById(R.id.listView2);
        int count = JNIServerLib.getOnlinePanoCamCount();
        for (int i = 0; i < count; i++) {
            String name = "unknow";
            try {
                name = new String(JNIServerLib.getOnlinePanoCamNameWithIndex(i),"GB2312");
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
            fileList.add(name);
        }
        adapter = new ArrayAdapter<String>(this, android.R.layout.simple_expandable_list_item_1, fileList);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(adapterListener);

    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK){
            disconnect(null);
        }
        return false;
    }

    public void disconnect(View btn) {
        new AlertDialog.Builder(this)
                .setTitle("断开连接")
                .setMessage("确定要断开连接吗?")
                .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
//                        JNIServerLib.unregistPano();
                        self.get().finish();
                    }
                })
                .setNegativeButton("取消", null)
                .setCancelable(false)
                .show();
    }

    final private AdapterView.OnItemClickListener adapterListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
            final int row = (int) l;
            Intent intent = new Intent(self.get(), LoadingActivity.class);
            startActivity(intent);
            new Thread(new Runnable() {
                @Override
                public void run() {
                    while (LoadingActivity.loadingActivity == null) ;
                    LoadingActivity.loadingActivity.startPos();
                    if (JNIServerLib.registOnlinePanoCamWithIndex(row) == true)
                    {
                        Intent intent = new Intent(self.get(), OnlinePlayActivity.class);
                        Bundle bundle = new Bundle();
                        bundle.putSerializable("NAME", fileList.get(row));
                        intent.putExtras(bundle);
                        mHandler.sendEmptyMessage(1);
                        startActivity(intent);
                        finish();
                    }
                    else {
                        mHandler.sendEmptyMessage(2);
                    }
                }
            }).start();

        }
    };

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
                            .setMessage("操作失败！\n不支持的相机或访问存储设备失败。\n可能是由于安全软件禁止访问权限。")
                            .setPositiveButton("确定", null)
                            .setCancelable(false)
                            .show();
                default :
                    break;
            }
        }
    };



}
