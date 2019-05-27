package com.panoeye.peplayer;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.Toast;

import com.panoeye.peplayer.peonline.ConnectManager;

import panoeye.pelibrary.Define;


public class ConnectPage extends AppCompatActivity {
    private static final String TAG = "ConnectPage";
    EditText IPAddr;
    LinearLayout layout;
    EditText serialNo;
    public static ConnectManager connectManager = null;
    static {
        System.loadLibrary("avcodec-57");
//        System.loadLibrary("native-lib");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }
    public native static int Init(int cameraCount);

//    public native static int RTSPInit(int cameraCount);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connect_page);
        IPAddr = findViewById(R.id.IPAddress);
        layout = findViewById(R.id.ctrl);
        serialNo = findViewById(R.id.serialNo);



        Spinner spinner = findViewById(R.id.spinner);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String[] cameraTypes = getResources().getStringArray(R.array.cameraTypes) ;
//                Toast.makeText(ConnectPage.this, "你点击的是:"+cameraTypes[position], Toast.LENGTH_SHORT).show();
                switch (cameraTypes[position]){
                    case "8路全景":
                        Define.cameraCount = 8;
                        IPAddr.setText("192.168.0.159");
                        serialNo.setText("E30010460");
                        layout.setVisibility(View.VISIBLE);
                        break;
                    case "5路全景":
                        Define.cameraCount = 5;
                        IPAddr.setText("192.168.20.168");
                        serialNo.setText("D00010368");//D20010020
                        layout.setVisibility(View.VISIBLE);
                        break;
                    case "环视双鱼眼":
                        Define.cameraCount = 2;
                        IPAddr.setText("192.168.20.177");
                        serialNo.setText("");
                        layout.setVisibility(View.INVISIBLE);
                        Define.isHalfBall = false;
                        break;
                    default:
                        Define.cameraCount = 1;
                        IPAddr.setText("192.168.20.239");
//                        IPAddr.setText("192.168.111.11");
                        serialNo.setText("");
                        layout.setVisibility(View.INVISIBLE);
                        Define.isHalfBall = true;
//                        Toast.makeText(ConnectPage.this, "选择了其他表示播放一个模组！", Toast.LENGTH_SHORT).show();
                        break;
                }
                Log.d(TAG, "onItemSelected: 相机路数："+Define.cameraCount);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                Toast.makeText(ConnectPage.this, "你未选择相机型号！", Toast.LENGTH_SHORT).show();
            }
        });

        //GetIP(null);
    }

    final private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case 0:
                    pd.dismiss();
                    Toast.makeText(ConnectPage.this," 相机连接成功！",Toast.LENGTH_SHORT).show();
                    break;
                case 1:
                    pd.dismiss();
                    AlertDialog.Builder builder = new AlertDialog.Builder(ConnectPage.this);  //先得到构造器
                    builder.setTitle("错误提示"); //设置标题
                    builder.setMessage("网络连接错误，请检查网络连接！"); //设置内容
                    //builder.setIcon(R.mipmap.ic_launcher);//设置图标，图片id即可
                    builder.create();
                    builder.create().show();
                    break;
                case 2:
                    pd.dismiss();
                    connectManager.closeRTSP();

                    builder = new AlertDialog.Builder(ConnectPage.this);  //先得到构造器
                    builder.setTitle("超時提示"); //设置标题
                    builder.setMessage("网络连接超时，请重试！"); //设置内容
                    //builder.setIcon(R.mipmap.ic_launcher);//设置图标，图片id即可
                    builder.create();
                    builder.create().show();
                    break;
            }
        }
    };
    private ProgressDialog pd;
    public void GetIP(View view) {
        pd = ProgressDialog.show(this,"loading...","please wait...");

        String head = "rtsp://admin:123456@";
        String IP = IPAddr.getText().toString();
        int port = 12341;
        int[] port5 = {12341,12344,12343,12342,12345};
        int[] port8 = {12341,12347,12346,12345,12344,12343,12342,12348};
        String end = "/mpeg4";
        int fishPort = 554;
        String fishEnd = "/profile2";
        final String[] rtspAddr =new String[Define.cameraCount];

        for (int cid=0;cid<Define.cameraCount;cid++){
            switch (Define.cameraCount){
                case 8:
                    rtspAddr[cid] = head+IP+":"+ port8[cid] +end;
                    break;
                case 5:
                    rtspAddr[cid] = head+IP+":"+ port5[cid] +end;
                    break;
                case 1:
                    rtspAddr[cid] = head+IP+":"+ fishPort +fishEnd;
//                    rtspAddr[cid] = head+IP+":"+ port +end;
                    break;
                default:
                    rtspAddr[cid] = head+IP+":"+ port +end;
                    Log.d(TAG, "port: "+port);
                    port++;
                    break;
            }
        }

        final Thread connThread = new Thread(new Runnable() {
            @Override
            public void run() {
//                android.os.Process.setThreadPriority (android.os.Process.THREAD_PRIORITY_LOWEST);
//                android.os.Process.setThreadPriority (android.os.Process.THREAD_PRIORITY_BACKGROUND);
//                Thread.currentThread().setPriority(4);
//                Log.e(TAG, "run:getPriority(): "+Thread.currentThread().getPriority());
                Init(Define.cameraCount);

                connectManager = new ConnectManager();
//                for (Integer cid:cidList){
                for (int cid=0;cid<Define.cameraCount;cid++){
                    final int finalHandle = cid;
                    new Thread(new Runnable(){
                        @Override
                        public void run() {
//                            android.os.Process.setThreadPriority (android.os.Process.THREAD_PRIORITY_BACKGROUND);
//                            Thread.currentThread().setPriority(6);
//                            Log.d(TAG, "run: getPriority(): "+Thread.currentThread().getPriority()+";cid:"+finalHandle);
                            int ret = connectManager.InitRTSP(rtspAddr[finalHandle],finalHandle);
                            Log.d(TAG, "run: cid:"+finalHandle+";ret-->"+ret);
                            if (ret == 0){
                                connectManager.flagsDict.put(finalHandle,"Succeed");
                                Log.d(TAG,"模组"+ finalHandle +"-->RTSPConnect()成功!");
                            }else {
                                connectManager.flagsDict.put(finalHandle,"Failed");
                                Log.e(TAG,"模组"+ finalHandle +"-->RTSPConnect()失败!");
                                connectManager.connErrCnt++;
                            }
                        }
                    }).start();
                }

                int sleepCount = 0;
                while (true){
                    Log.d(TAG, "onCreate: true循环，连接相机中...");
                    Log.d(TAG, "RTSPConnect(): flags.size()-->:"+connectManager.flagsDict.size());
                    if (connectManager.flagsDict.size() == Define.cameraCount){
                        if (connectManager.connErrCnt != Define.cameraCount){
                            Intent intent = new Intent(ConnectPage.this,PlayOnline.class);
                            Bundle bundle = new Bundle();
                            bundle.putSerializable("serialNo",serialNo.getText().toString());
                            intent.putExtras(bundle);
                            startActivity(intent);
                            handler.sendEmptyMessage(0);
                        }else {
                            handler.sendEmptyMessage(1);
                        }
                        break;
                    }
                    try {
                        sleepCount++;
                        Log.d(TAG, "onCreate: sleepCount:"+sleepCount);
                        if (sleepCount>=5){
                            handler.sendEmptyMessage(2);
                            break;
                        }
//                        Thread.yield();
                        Thread.sleep(2000);//线程休眠2s
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
//        connThread.setPriority(4);
        connThread.start();
    }

    public void back(View view) {
        this.finish();
    }
}
