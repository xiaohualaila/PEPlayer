package com.panoeye.peplayer;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

import panoeye.pelibrary.Define;
import panoeye.pelibrary.PEDecodeManagerStatus;
import panoeye.pelibrary.PEError;
import panoeye.pelibrary.PEManager;
import panoeye.pelibrary.PesHandle;

/**
 * Created by Administrator on 2017/10/11.
 */

public class PlayAVS extends AppCompatActivity implements SensorEventListener {
    static PesHandle pesHandle = null;
    GLSurfaceView view = null;
    Timer time;//计时器
    Button showMode, btnVR,btnFlip;//显示模式
    private static final String TAG = "PlayAVS";

    private SeekBar seekBar;//拖动条
    RelativeLayout layout;//控制块

    private SensorManager sensorManager;
    private Sensor gyroscopeSensor;         //陀螺仪
    private Sensor accelerometerSensor;     //加速度
    private Sensor linearAccelerometerSensor;     //线加速度

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //关闭状态栏，全屏查看回放(失效)
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);//关闭状态栏
//        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);//横屏查看
        //关闭标题栏(无效)
//        requestWindowFeature(Window.FEATURE_NO_TITLE);
//        getWindow().requestFeature(Window.FEATURE_NO_TITLE);

        setContentView(R.layout.activity_play_av_pes);

        sensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
//        gyroscopeSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);//陀螺仪
//        accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);//加速度
//        linearAccelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);//线加速度
        getSensorService();

        view = (GLSurfaceView)findViewById(R.id.surfaceView);

        //String name = (String)getIntent().getSerializableExtra("NAME");
        try {
            //panoHandle = PESManager.buildPano(name);
            PEManager.setView(pesHandle,view);
            setView();
            PEManager.start(pesHandle);
            Log.d("start", "onCreate: ");
        }catch (PEError e){

        }

        TimerTask task = new TimerTask() {//计时器任务
            @Override
            public void run() {
                handler.sendEmptyMessage(0);
                handler.sendEmptyMessage(1);
            }
        };
        time = new Timer();//初始化计时器
        time.schedule(task,0,1000);//计时器时间表：每1s执行一次task任务
    }

    //pitch  俯仰角    yaw 偏航角     roll 滚转角
    float angleYaw = 0, anglePitch = 0, angleRoll = 0;
    float angleYawVROffset = 0;
    float radiansYaw = 0, radiansPitch = 0;
    long sensorTS = 0;
    float speedX = 0,speedZ = 0,speedY = 0;
    @Override
    public void onSensorChanged(SensorEvent event) {
//        float dx = event.values[0];
//        float dy = event.values[1];
//        float dz = event.values[2];
//        if (vrFlag == true) {
//            if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
//                float pitch = (float) Math.toDegrees(Math.atan(dx / dz));
//                if (pitch < 0) {
//                    pitch += 180;
//                }
//                pitch = Math.min(Math.max(pitch, PITCH_MIN), PITCH_MAX);
//                float roll = (float) Math.toDegrees(Math.atan((dy - speedY) / dx));
//                roll = Math.min(Math.max(roll, -85), 85);
//                //非极端俯仰和横滚下
//                if ((pitch != PITCH_MIN && pitch != PITCH_MAX) && (roll != -85 && roll != 85)) {
//                    //参与辅助校准
//                    radiansPitch = (radiansPitch * 3 + (float) Math.toRadians(pitch)) / 4;
//                    //直接计算角度
//                    angleRoll = (angleRoll * 3 + roll) / 4;
//                }
//            } else if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
//                speedY = dy;
//            }else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
//                if (sensorTS != 0) {
//                    double dT = (event.timestamp - sensorTS) / 1000000000.0f;
//                    float radiansYawDifference = (float)(dx * dT);
//                    float radiansPitchDifference = (float)(dy * dT);
//                    radiansYaw += radiansYawDifference;
//                    float angleXAim = (float) Math.toDegrees(radiansYaw);
//                    radiansPitch -= radiansPitchDifference;
//                    float angleYAim = (float) Math.toDegrees(radiansPitch);
//                    angleYAim = Math.min(Math.max(angleYAim, PITCH_MIN), PITCH_MAX);
//                    //滤波
//                    angleYaw = (angleYaw * 3 + angleXAim) / 4;
//                    anglePitch = (anglePitch * 3 + angleYAim) / 4;
//
//                }
//                sensorTS = event.timestamp;
//            }
//        }
//        Define.x = angleYaw + angleYawVROffset;
//        Define.y = anglePitch;
//        //Log.d(TAG, "onSensorChanged: anglePitch"+anglePitch);
//        Define.z = angleRoll;
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    private float[] accelerometerValues;
    private float[] magnetFieldValues;
    private final SensorEventListener sensorEventListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            float dx = event.values[0];
            float dy = event.values[1];
            float dz = event.values[2];
            if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                accelerometerValues = event.values;
            }
            if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
                magnetFieldValues = event.values;
            }
            if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
                speedY = dy;
                speedZ = dz;
                speedX = dx;
                //Log.d(TAG, "LINEAR: "+speedY);
                //Log.d(TAG, "LINEAR: "+speedZ);
                //Log.d(TAG, "LINEAR: "+speedX);
            }
            if (accelerometerValues != null &&magnetFieldValues != null) {
                float[] values = new float[3];
                float[] R = new float[9];
                SensorManager.getRotationMatrix(R, null,
                        accelerometerValues, magnetFieldValues);
                SensorManager.getOrientation(R, values);
                values[0] = (float) Math.toDegrees(values[0]);
                values[1] = (float) Math.toDegrees(values[1]);
                values[2] = (float) Math.toDegrees(values[2]);
                    /* 三轴方向 */
                if (vrFlag){
                    Define.x = (-values[0]*3+Define.x)/4;//yaw
                    Define.y = (-values[2]*3+Define.y)/4;//pitch
                    Define.z = (-values[1]*3+Define.z)/4;//roll
                }
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };

    private void getSensorService() {
//      int rate=SensorManager.SENSOR_DELAY_FASTEST;
        int rate=SensorManager.SENSOR_DELAY_GAME;
        Sensor accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        sensorManager.registerListener(sensorEventListener, accelerometerSensor, rate);

        Sensor magnetSensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        sensorManager.registerListener(sensorEventListener, magnetSensor, rate);

        linearAccelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        sensorManager.registerListener(sensorEventListener, linearAccelerometerSensor, rate);

//        gyroscopeSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
//        sensorManager.registerListener(sensorEventListener, gyroscopeSensor,rate);
    }

    public void playBtn(View btn){
        try {
            if (PEManager.getStatus(pesHandle)== PEDecodeManagerStatus.play){
                PEManager.pause(pesHandle);
                playBtn.setBackground(getResources().getDrawable(R.drawable.ic_media_play));
            }
            else if (PEManager.getStatus(pesHandle)== PEDecodeManagerStatus.pause){
                PEManager.reView(pesHandle);
                playBtn.setBackground(getResources().getDrawable(R.drawable.ic_media_pause));
            }
            else if(PEManager.getStatus(pesHandle)== PEDecodeManagerStatus.over){
                PEManager.restart(pesHandle);
                playBtn.setBackground(getResources().getDrawable(R.drawable.ic_media_pause));
            }else {

            }
        }catch (PEError e){

        }
    }

    Button playBtn;
    TextView currentTimeLable,maxTimeLabel,fps ,cpu,video;
    private void setView(){
//        hideOrShow = true;
        playBtn = (Button)findViewById(R.id.playBtn);
        layout = (RelativeLayout)findViewById(R.id.ctrl);
        seekBar = (SeekBar)findViewById(R.id.seekBar);
        seekBar.setMax((int)(pesHandle.duration.floatValue()));
        currentTimeLable = (TextView)findViewById(R.id.currentTimeLabel);
        maxTimeLabel = (TextView)findViewById(R.id.maxTimeLabel);
        maxTimeLabel.setText(getTimeStr(pesHandle.duration.floatValue()));
        fps = (TextView)findViewById(R.id.fps);
        video = (TextView)findViewById(R.id.video);
        cpu = (TextView) findViewById(R.id.cpu);
        showMode = (Button)findViewById(R.id.showMode);
        btnVR = findViewById(R.id.btnVR);
        btnFlip = findViewById(R.id.btnFlip);

        //拖动进度条的事件监听需要实现SeekBar.OnSeekBarChangeListener接口
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override//该方法拖动进度条进度改变的时候调用
            public void onProgressChanged(SeekBar seekBar, int curTime, boolean fromUser) {
                if (fromUser){
                    try {
                        PEManager.jumpToTime(pesHandle, (float) curTime);
                        currentTimeLable.setText(getTimeStr(curTime));
                    }catch (PEError e){

                    }
                }
            }

            @Override//该方法拖动进度条开始拖动的时候调用
            public void onStartTrackingTouch(SeekBar seekBar) {
                try {
                    PEManager.pause(pesHandle);
                    playBtn.setBackground(getResources().getDrawable(R.drawable.ic_media_play));
                }catch (PEError e){

                }
            }

            @Override//该方法拖动进度条停止拖动的时候调用
            public void onStopTrackingTouch(SeekBar seekBar) {
                try {
                    PEManager.reView(pesHandle);
                    playBtn.setBackground(getResources().getDrawable(R.drawable.ic_media_pause));
                }catch (PEError e){

                }
            }
        });
    }

    private String getTimeStr(float time){
        String timeText = "";
        if (time / 60 < 10) {
            timeText += "0";
        }
        timeText += String.valueOf((int)(time / 60));
        timeText += ":";
        if (time % 60 < 10) {
            timeText += "0";
        }
        timeText += String.valueOf((int)(time % 60));
        return timeText;
    }

    Handler handler = new Handler(){
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 0:
                    currentTimeLable.setText(getTimeStr(PEManager.getTimeStamp(pesHandle)));//设置当前正在播放的时间
                    seekBar.setProgress((PEManager.getTimeStamp(pesHandle).intValue()));//设置时间轴当前游标的位置
                    try {
                        if (PEManager.getStatus(pesHandle)== PEDecodeManagerStatus.over){
                            playBtn.setBackground(getResources().getDrawable(R.drawable.ic_media_play));
                        }
                    }catch (PEError e){

                    }
                    break;
                case 1:
                    fps.setText("OpenGL:"+ Define.openglFrameRate + "fps");
                    Define.openglFrameRate=0;//归零
                    video.setText("Videos:"+Define.videoFrameRate/ pesHandle.camCount + "fps");
                    Define.videoFrameRate=0;//归零
                    cpu.setText("CPU: " + String.valueOf(((int)(getAppCpuRate() * 100)) / 100.0) + " %");
                    break;
            }
        }
    };

    private float spacing(MotionEvent event){
        float x = event.getX(0) - event.getX(1);
        float y = event.getY(0) - event.getY(1);
        return (float) (Math.sqrt(x*x+y*y));
    }
    boolean isZoomming = false;
    boolean isDragging = false;
    float newDir;
    float mPreviousX,mPreviousY;
    float mPreviousDir;
    static final float FOVY_MIN = 5.0f;
    static final float FOVY_MAX = 90.0f;
//    float angleYawVROffset = 0;
    Date touchDownTime;
    boolean hideOrShow;
    //RelativeLayout layout;
    public boolean onTouchEvent(MotionEvent event){
        float x = event.getX();
        float y = event.getY();
        switch (event.getAction()&MotionEvent.ACTION_MASK){
            case MotionEvent.ACTION_MOVE:
                if (isZoomming){
                    newDir = spacing(event);
                    float tScale =  (float)(Define.fovy - (Math.min(Math.max(newDir / mPreviousDir, 0.6), 1.4) - 1) * 10 * 2);
                    Define.fovy = Math.min(Math.max((tScale), FOVY_MIN), FOVY_MAX);
                    Log.d("mPreviousDir", String.valueOf(newDir / mPreviousDir));
                    mPreviousDir = newDir;
                    break;
                }

                if (isDragging){
                    float dx = (x - mPreviousX)/10;
                    float dy = (y - mPreviousY)/10;
                    Define.x += dx;
                    Define.y += dy;
                    Log.d(TAG, "onTouchEvent: x"+Define.x);
                    Log.d(TAG, "onTouchEvent: y"+Define.y);
                    angleYawVROffset += dx;
                    mPreviousY = y;
                    mPreviousX = x;
                    break;
                }

            case MotionEvent.ACTION_DOWN:
                mPreviousX = x;
                mPreviousY = y;
                touchDownTime = new Date();
                isDragging = true;
                break;
            case MotionEvent.ACTION_UP:
                isDragging = false;
                isZoomming = false;
                if (touchDownTime!=null){
                    Date nowTime = new Date();
                    if (nowTime.getTime() - touchDownTime.getTime()<=100){
                        if (hideOrShow == true){
                            hideOrShow = false;
                            layout.setVisibility(View.INVISIBLE);
                        }else {
                            hideOrShow = true;
                            layout.setVisibility(View.VISIBLE);
                        }
                    }
                }
                break;
            case MotionEvent.ACTION_POINTER_UP:
                isDragging = false;
                isZoomming = false;
                touchDownTime= null;
                break;
            case MotionEvent.ACTION_POINTER_DOWN:
                touchDownTime= null;
                if (event.getPointerCount() == 2) {
                    isZoomming = true;
                    mPreviousDir = spacing(event);
                }
                break;
        }

        return true;
    }

    //监听Back键按下事件
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK&&event.getRepeatCount()==0){
            try {
                time.cancel();
                sensorManager.unregisterListener(this);
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
                PEManager.invalidate(pesHandle);
                view = null;
                pesHandle = null;
            }catch (PEError e){
            }
            this.finish();
        }
        return true;
    }

    float lastTotalCpuTime = -1;
    float lastAppCpuTime = -1;
    public float getAppCpuRate() {

        float totalCpuTime = getTotalCpuTime();
        float appCpuTime = getAppCpuTime();
        float cpuRate;
        if (lastTotalCpuTime != -1 && lastAppCpuTime != -1) {
            cpuRate = 100 * (appCpuTime - lastAppCpuTime)
                    / (totalCpuTime - lastTotalCpuTime);
        } else {
            cpuRate = 0;
        }
        lastTotalCpuTime = totalCpuTime;
        lastAppCpuTime = appCpuTime;
        return cpuRate;
    }

    public long getTotalCpuTime() { // 获取系统总CPU使用时间
        String[] cpuInfos = null;
        try
        {
            BufferedReader reader = new BufferedReader(new InputStreamReader(
                    new FileInputStream("/proc/stat")), 1000);
            String load = reader.readLine();
            reader.close();
            cpuInfos = load.split(" ");
        }
        catch (IOException ex)
        {
            ex.printStackTrace();
            return -1;
        }
        long totalCpu = Long.parseLong(cpuInfos[2])
                + Long.parseLong(cpuInfos[3]) + Long.parseLong(cpuInfos[4])
                + Long.parseLong(cpuInfos[6]) + Long.parseLong(cpuInfos[5])
                + Long.parseLong(cpuInfos[7]) + Long.parseLong(cpuInfos[8]);
        return totalCpu;
    }

    public long getAppCpuTime() { // 获取应用占用的CPU时间
        String[] cpuInfos = null;
        try
        {
            int pid = android.os.Process.myPid();
            BufferedReader reader = new BufferedReader(new InputStreamReader(
                    new FileInputStream("/proc/" + pid + "/stat")), 1000);
            String load = reader.readLine();
            reader.close();
            cpuInfos = load.split(" ");
        }
        catch (IOException ex)
        {
            ex.printStackTrace();
        }
        long appCpuTime = Long.parseLong(cpuInfos[13])
                + Long.parseLong(cpuInfos[14]) + Long.parseLong(cpuInfos[15])
                + Long.parseLong(cpuInfos[16]);
        return appCpuTime;
    }

    boolean vrFlag = false;
    public void setVR(View btn){
        if (vrFlag == false){
            PEManager.setVrMode();
            btnVR.setText("全景观看");
            showMode.setEnabled(false);
            angleYawVROffset = 0;
            angleYaw = 0;
            radiansYaw = 0;
            sensorTS = 0;
            Define.fovy = 45f;
//            sensorManager.registerListener(this, gyroscopeSensor, SensorManager.SENSOR_DELAY_GAME);
//            sensorManager.registerListener(this, accelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
//            sensorManager.registerListener(this, linearAccelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
            vrFlag = true;
        }else {
            PEManager.setNormal();
            btnVR.setText("VR观看");
            showMode.setEnabled(true);
            Define.x = 0;
            Define.y = 0;
            Define.z = 0;
//            sensorManager.unregisterListener(this, gyroscopeSensor);
//            sensorManager.unregisterListener(this, accelerometerSensor);
//            sensorManager.unregisterListener(this, linearAccelerometerSensor);
            angleRoll = 0;
            vrFlag = false;
        }
    }
    boolean flipFlag = true;
    public void setFlip(View btn) {
        if (flipFlag == false){
            PEManager.setFlip(false);
            flipFlag = true;
        }else {
            PEManager.setFlip(true);
            flipFlag = false;
        }
    }

    public void showOriginalView(View view) {
        if (Define.mode == Define.MODE_GLOBULAR) {
            Define.mode = Define.MODE_ORIGINAL;
            showMode.setText("原始图像");
            btnVR.setEnabled(false);
            btnFlip.setEnabled(false);
        }else {
            Define.mode = Define.MODE_GLOBULAR;
            showMode.setText("全景图像");
            btnVR.setEnabled(true);
            btnFlip.setEnabled(true);
        }
        Define.y = 0;
        Define.x = 0;
        Define.z = 0;
    }
}