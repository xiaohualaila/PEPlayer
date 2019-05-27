//package com.panoeye.peplayer.peonline;
//
//import android.opengl.GLSurfaceView;
//import android.os.Handler;
//import android.os.Message;
//import android.support.v7.app.AppCompatActivity;
//import android.os.Bundle;
//import android.util.Log;
//import android.view.KeyEvent;
//import android.view.MotionEvent;
//import android.view.View;
//import android.view.WindowManager;
//import android.widget.Button;
//import android.widget.TextView;
//import android.widget.Toast;
//
//import com.panoeye.peplayer.R;
//
//import java.io.BufferedReader;
//import java.io.FileInputStream;
//import java.io.IOException;
//import java.io.InputStreamReader;
//import java.net.InetSocketAddress;
//import java.net.Socket;
//import java.net.SocketAddress;
//import java.sql.Time;
//import java.util.Date;
//import java.util.Timer;
//import java.util.TimerTask;
//
//public class FishEyeDrawActivity extends AppCompatActivity {
//    GLSurfaceView view = null;
//    FEDrawManager drawManager1,drawManager2;
//    Timer time;
//    Socket socket = null;
//    FEFrameBuffer []frameBuffer;
//    FEDecodeBuffer []decodeBuffer;
//    static {
//        System.loadLibrary("avcodec-57");
//        System.loadLibrary("native-lib");
//        System.loadLibrary("avfilter-6");
//        System.loadLibrary("avformat-57");
//        System.loadLibrary("avutil-55");
//        System.loadLibrary("swresample-2");
//        System.loadLibrary("swscale-4");
//    }
//    public native static int Init();
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
//                WindowManager.LayoutParams.FLAG_FULLSCREEN);
//        setContentView(R.layout.activity_draw_fisheye);
//
//        view = (GLSurfaceView)findViewById(R.id.surfaceView);
//
//        String RTSPAddr1 = (String)getIntent().getSerializableExtra("RTSPAddr1");
//        String RTSPAddr2 = (String)getIntent().getSerializableExtra("RTSPAddr2");
//
//        frameBuffer = new FEFrameBuffer[2];
//        decodeBuffer = new FEDecodeBuffer[2];
//
//        new Thread(new Runnable() {
//            @Override
//            public void run() {
//                Log.i("cj", "linkBtn: ");
//                String ip = (String) getIntent().getSerializableExtra("ip");
//                String port = (String) getIntent().getSerializableExtra("port");
//                try {
//                    socket = new Socket();
//                    String portstr = port;
//                    int portNum = Integer.parseInt(portstr);
//                    SocketAddress add = new InetSocketAddress(ip.toString(),portNum);
//                    socket.connect(add,5000);
//                    //socket = new Socket("192.168.111.199",5000);
//                    BufferedReader buff = new BufferedReader(new InputStreamReader(
//                            socket.getInputStream()
//                    ));
//                    String line = null;
//                    while ((line = buff.readLine())!=null){
//                        Message msg = new Message();
//                        msg.what = 0x11;
//                        Bundle bundle = new Bundle();
//                        bundle.clear();
//
//                        int headCH = line.indexOf('X');
//                        if (headCH>=0)
//                        {
//                            line = line.substring(headCH);
//                            char ch = line.charAt(8);
//                            if (ch=='Y'){
//                                //Log.e(String.valueOf(line.length()),line+"\n");
//                                bundle.putString("msg",line);
//                                msg.setData(bundle);
//                                handler.sendMessage(msg);
//                            }
//                        }
//                    }
//
//                    buff.close();
//                    socket.close();
//                }catch (IOException e){
//                    e.printStackTrace();
//                    Bundle bundle = new Bundle();
//                    bundle.clear();
//                    Message msg = new Message();
//                    msg.what = 0x11;
//                    bundle.putString("msg","陀螺仪获取错误");
//                    msg.setData(bundle);
//                    handler.sendMessage(msg);
//                }
//            }
//        }).start();
//
//        Init();
//        if (RTSPAddr1.equals("") == false&&RTSPAddr2.equals("")==true){
//            frameBuffer[0] = new FEFrameBuffer();
//            drawManager1 = new FEDrawManager(RTSPAddr1,frameBuffer[0]);
//
//            boolean ret = drawManager1.Init();
//            if (ret == false){
//
//            }else {
//
//            }
//            drawManager1.start();
//            decodeBuffer[0] = new FEDecodeBuffer(frameBuffer[0]);
//            Define.isHalfBall = true;
//        }
//        if (RTSPAddr1.equals("") == true &&RTSPAddr2.equals("")==false){
//            frameBuffer[0] = new FEFrameBuffer();
//            drawManager2 = new FEDrawManager(RTSPAddr2,frameBuffer[0]);
//            boolean ret = drawManager2.Init();
//            if (ret == false){
//
//            }else {
//
//            }
//
//            drawManager2.start();
//            decodeBuffer[0] = new FEDecodeBuffer(frameBuffer[0]);
//            Define.isHalfBall = true;
//        }
//        if (RTSPAddr1.equals("") == true&&RTSPAddr2.equals("") == true){
//
//        }
//        if (RTSPAddr1.equals("") == false&&RTSPAddr2.equals("") == false){
//            Define.isHalfBall = false;
//
//            frameBuffer[0] = new FEFrameBuffer();
//            drawManager1 = new FEDrawManager(RTSPAddr1,frameBuffer[0]);
//            boolean ret = drawManager1.Init();
//            if (ret == false){
//
//            }
//            else {
//
//            }
//
//
//            frameBuffer[1] = new FEFrameBuffer();
//            drawManager2 = new FEDrawManager(RTSPAddr2,frameBuffer[1]);
//            ret = drawManager2.Init();
//            if (ret == false){
//
//            }else {
//
//            }
//
//            decodeBuffer[0] = new FEDecodeBuffer(frameBuffer[0]);
//            decodeBuffer[1] = new FEDecodeBuffer(frameBuffer[1]);
//
//            drawManager2.start();
//            drawManager1.start();
//
//        }
//
//        FEDraw draw = new FEDraw(view,decodeBuffer,getApplicationContext());
//        draw.draw();
//
//        TimerTask task = new TimerTask() {
//            @Override
//            public void run() {
//                handler.sendEmptyMessage(0);
//            }
//        };
//        time = new Timer();
//        time.schedule(task,0,1000);
//        setView();
//    }
//
//    TextView fps1,video1,video2,cpu,gyro;
//    void setView(){
//        fps1 = (TextView)findViewById(R.id.fps1);
//        video2 = (TextView)findViewById(R.id.video2);
//        video1 = (TextView)findViewById(R.id.video1);
//        cpu = (TextView)findViewById(R.id.cpu);
//        proBtn = (Button)findViewById(R.id.proBtn);
//        posBtn = (Button)findViewById(R.id.posBtn);
//        Flip = (Button)findViewById(R.id.Flip);
//        Flip.setEnabled(false);
//        gyro = (TextView)findViewById(R.id.gyro);
//    }
//    Handler handler = new Handler(){
//        @Override
//        public void handleMessage(Message msg) {
//            switch (msg.what){
//                case 0:
//                    fps1.setText("fps1:"+Define.fps1);
//                    video2.setText("video2:"+Define.video2);
//                    video1.setText("video1:"+Define.video1);
//                    cpu.setText("CPU: " + String.valueOf(((int)(getAppCpuRate() * 100)) / 100.0) + " %");
//                    Define.fps1 = 0;
//                    Define.video2 = 0;
//                    Define.video1 = 0;
//                case 0x11:
//                    Bundle bundle = msg.getData();
//                    if (bundle.getString("msg")!=null){
//                        gyro.setText(bundle.getString("msg"));
//                    }
//                    break;
//                default:
//                    break;
//            }
//
//
//        }
//    };
//
//    float lastTotalCpuTime = -1;
//    float lastAppCpuTime = -1;
//    public float getAppCpuRate() {
//
//        float totalCpuTime = getTotalCpuTime();
//        float appCpuTime = getAppCpuTime();
//        float cpuRate;
//        if (lastTotalCpuTime != -1 && lastAppCpuTime != -1) {
//            cpuRate = 100 * (appCpuTime - lastAppCpuTime)
//                    / (totalCpuTime - lastTotalCpuTime);
//        } else {
//            cpuRate = 0;
//        }
//        lastTotalCpuTime = totalCpuTime;
//        lastAppCpuTime = appCpuTime;
//        return cpuRate;
//    }
//
//    public long getTotalCpuTime() { // 获取系统总CPU使用时间
//        String[] cpuInfos = null;
//        try
//        {
//            BufferedReader reader = new BufferedReader(new InputStreamReader(
//                    new FileInputStream("/proc/stat")), 1000);
//            String load = reader.readLine();
//            reader.close();
//            cpuInfos = load.split(" ");
//        }
//        catch (IOException ex)
//        {
//            ex.printStackTrace();
//        }
//        long totalCpu = Long.parseLong(cpuInfos[2])
//                + Long.parseLong(cpuInfos[3]) + Long.parseLong(cpuInfos[4])
//                + Long.parseLong(cpuInfos[6]) + Long.parseLong(cpuInfos[5])
//                + Long.parseLong(cpuInfos[7]) + Long.parseLong(cpuInfos[8]);
//        return totalCpu;
//    }
//
//    public long getAppCpuTime() { // 获取应用占用的CPU时间
//        String[] cpuInfos = null;
//        try
//        {
//            int pid = android.os.Process.myPid();
//            BufferedReader reader = new BufferedReader(new InputStreamReader(
//                    new FileInputStream("/proc/" + pid + "/stat")), 1000);
//            String load = reader.readLine();
//            reader.close();
//            cpuInfos = load.split(" ");
//        }
//        catch (IOException ex)
//        {
//            ex.printStackTrace();
//        }
//        long appCpuTime = Long.parseLong(cpuInfos[13])
//                + Long.parseLong(cpuInfos[14]) + Long.parseLong(cpuInfos[15])
//                + Long.parseLong(cpuInfos[16]);
//        return appCpuTime;
//    }
//
//    @Override
//    public boolean onKeyDown(int keyCode, KeyEvent event) {
//        if(keyCode == KeyEvent.KEYCODE_BACK&&event.getRepeatCount()==0){
//            drawManager1.stop();
//            drawManager2.stop();
//            view = null;
//            drawManager1 = null;
//            drawManager2 = null;
//            time.cancel();
//            decodeBuffer = null;
//            frameBuffer = null;
//            finish();
//        }
//        return true;
//    }
//
//    Button proBtn,posBtn,Flip;
//    public void stretch(View btn){
//        if (Define.isStretch == true){
//            Define.x = 0;
//            Define.y = 0;
//            Define.fovy = 45;
//            proBtn.setText("柱面投影");
//            posBtn.setEnabled(true);
//            Flip.setEnabled(false);
//            Define.isStretch = false;
//        }else {
//            Define.x = -200;
//            Define.y = -40;
//            Define.fovy = 65;
//            Flip.setEnabled(true);
//            posBtn.setEnabled(false);
//            proBtn.setText("球状投影");
//            Define.isStretch = true;
//        }
//    }
//    public void posBall(View btn){
//        if (Define.isInBall == false){
//            posBtn.setText("球外");
//            Define.isInBall = true;
//        }else {
//            posBtn.setText("球内");
//            Define.isInBall = false;
//        }
//    }
//    public void Flip(View btn){
//        if (Define.isFlip == false){
//            Define.isFlip = true;
//        }else {
//            Define.isFlip = false;
//        }
//    }
//
//    private float spacing(MotionEvent event){
//        float x = event.getX(0) - event.getX(1);
//        float y = event.getY(0) - event.getY(1);
//        return (float) (Math.sqrt(x*x+y*y));
//    }
//
//    float mPreviousX,mPreviousY;
//    float mPreviousDir;
//    boolean isDragging = false;
//    boolean isZoomming = false;
//    float newDir;
//    static final float FOVY_MIN = 5.0f;
//    static final float FOVY_MAX = 180.0f;
//    Date touchDownTime;
//    public boolean onTouchEvent(MotionEvent event){
//        float x = event.getX();
//        float y = event.getY();
//        switch (event.getAction()&MotionEvent.ACTION_MASK){
//            case MotionEvent.ACTION_MOVE:
//                if (isZoomming){
//                    newDir = spacing(event);
//                    float tScale =  (float)(Define.fovy - (Math.min(Math.max(newDir / mPreviousDir, 0.6), 1.4) - 1) * 10 * 2);
//                    Define.fovy = Math.min(Math.max((tScale), FOVY_MIN), FOVY_MAX);
//                    Log.d("mPreviousDir", String.valueOf(newDir / mPreviousDir));
//                    mPreviousDir = newDir;
//                    break;
//                }
//
//                if (isDragging){
//                    float dx = (x - mPreviousX)/10;
//                    float dy = (y - mPreviousY)/10;
//                    Define.x += dx;
//                    Define.y += dy;
//                    mPreviousY = y;
//                    mPreviousX = x;
//                    break;
//                }
//
//            case MotionEvent.ACTION_DOWN:
//                mPreviousX = x;
//                mPreviousY = y;
//                touchDownTime = new Date();
//                isDragging = true;
//                break;
//            case MotionEvent.ACTION_UP:
//                isDragging = false;
//                isZoomming = false;
//
//                break;
//            case MotionEvent.ACTION_POINTER_UP:
//                isDragging = false;
//                isZoomming = false;
//                touchDownTime= null;
//                break;
//            case MotionEvent.ACTION_POINTER_DOWN:
//                touchDownTime= null;
//                if (event.getPointerCount() == 2) {
//                    isZoomming = true;
//                    mPreviousDir = spacing(event);
//                }
//                break;
//        }
//
//        return true;
//    }
//}
