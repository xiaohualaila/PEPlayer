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
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.panoeye.peplayer.peonline.FEDraw;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

import panoeye.pelibrary.Define;
import panoeye.pelibrary.PEBinFile;
import panoeye.pelibrary.PECmbFile;
import panoeye.pelibrary.PEColorTune;
import panoeye.pelibrary.PEDecodeBuffer;
import panoeye.pelibrary.PEError;
import panoeye.pelibrary.PEFrameBuffer;

import static com.panoeye.peplayer.ConnectPage.connectManager;
import static panoeye.pelibrary.PEColorTune.getGain;
import static panoeye.pelibrary.PEColorTune.initGain;
import static panoeye.pelibrary.PEColorTune.setGain;

//import static ConnectManager.flags;

/**
 * Created by Administrator on 2018/1/8.
 */

public class PlayOnline extends AppCompatActivity implements SensorEventListener {
    private static final String TAG = "PlayOnline";
    GLSurfaceView view = null;
    PEBinFile binFile;
//    ConnectManager connectManager = null;
    PEFrameBuffer frameBuffer;
    PEDecodeBuffer decodeBuffer;
    Timer time;
    Button showMode, btnVR,btnFlip,btnTurn;//显示模式
    RelativeLayout layout;//控制块
    public native String stringFromJNI();

    private SensorManager sensorManager;
    private Sensor gyroscopeSensor;         //陀螺仪
    private Sensor accelerometerSensor;     //加速度
    private Sensor linearAccelerometerSensor;     //线加速度

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: ");
        setContentView(R.layout.activity_play_online);
        setView();

        sensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
        gyroscopeSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);//陀螺仪
        accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);//加速度
        linearAccelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);//线加速度
//        getSensorService();

        view = findViewById(R.id.surfaceView);
        //读取bin文件
        try {
            String serial_no = getIntent().getSerializableExtra("serialNo").toString();
            Log.d(TAG, "run: serial_no:"+serial_no);
            if (serial_no.isEmpty()){
                binFile = null;
            }else {
                binFile = new PEBinFile(Define.PARAM_PATH+serial_no+".bin");
                //读取cmb文件
                String cmbPath = Define.PARAM_PATH+serial_no+".cmb";
                PECmbFile cmbFile = null;
                File cFile = new File(cmbPath);
                if (cFile.exists()) {
                    cmbFile = new PECmbFile(cmbPath);
                    new PEColorTune(cmbFile);
                    PEColorTune.isNeedOrgImg =true;
                    PEColorTune.orgImgReadyList = new int[]{0, 0, 0, 0, 0, 0, 0, 0};
                    PEColorTune.isGainReady = false;
                    Define.isCmbExist = true;
                }else {
                    Log.d(TAG, "onCreate: 找不到cmb调色文件！");
//                    Toast.makeText(this,"找不到cmb调色文件！",Toast.LENGTH_SHORT).show();
                }
            }
        } catch (PEError peError) {
            peError.printStackTrace();
        }

//        TextView tv = findViewById(R.id.video2);
//        tv.setText(stringFromJNI());

        connectManager.InitDecoder();
//        for (Integer cid:binFile.cidList){
            for (int cid=0;cid<Define.cameraCount;cid++){
            if (connectManager.flagsDict.get(cid) == "Succeed"){
                final int finalHandle = cid;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
//                        android.os.Process.setThreadPriority (android.os.Process.THREAD_PRIORITY_BACKGROUND);
                        connectManager.runLoop(finalHandle);
                    }
                }).start();
            }
        }
//        Log.e(TAG, "runLoop()执行结束！");

        if (binFile != null){
//            try {
//                decodeBuffer = new PEDecodeBuffer(binFile);
//            } catch (PEError peError) {
//                peError.printStackTrace();
//            }
//            FEDraw draw = new FEDraw(view,decodeBuffer,binFile);
            FEDraw draw = new FEDraw(view,connectManager.decodeBuffer,binFile);
            draw.drawPano();
        }else {
            layout.setVisibility(View.INVISIBLE);
//            decodeBuffer = new PEDecodeBuffer(Define.cameraCount);
            FEDraw draw = new FEDraw(view,connectManager.decodeBuffer,getApplicationContext());
//            if(Define.cameraCount == 2)
                draw.drawFishEye();
//            else
//                draw.drawRaw();
        }

        TimerTask task = new TimerTask() {
            @Override
            public void run() {
                handler.sendEmptyMessage(0);
            }
        };
        time = new Timer();
        time.schedule(task,0,1000);
    }
    TextView fps,videos,cpu;
    void setView(){
        fps = findViewById(R.id.fps);
        videos = findViewById(R.id.videos);
        cpu = findViewById(R.id.cpu);
        layout = findViewById(R.id.ctrl);
        showMode = findViewById(R.id.showMode);
        btnTurn = findViewById(R.id.colorTurn);
        btnVR = findViewById(R.id.btnVR);
        btnFlip = findViewById(R.id.btnFlip);
//        proBtn = (Button)findViewById(R.id.proBtn);
//        posBtn = (Button)findViewById(R.id.posBtn);
//        Flip = (Button)findViewById(R.id.Flip);
//        Flip.setEnabled(false);
        if (Define.isCmbExist){
            PEColorTune.colorTurnFlag = false;//重置调色状态
        }
    }
    Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case 0:
                    fps.setText("OpenGL:"+Define.openglFrameRate +"fps");
                    videos.setText("Videos:"+Define.videoFrameRate/Define.cameraCount+"fps");
                    cpu.setText("CPU: " + String.valueOf(((int)(getAppCpuRate() * 100)) / 100.0) + " %");
                    Define.openglFrameRate = 0;//归零
                    Define.videoFrameRate = 0; //归零
                    break;
            }
        }
    };
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
            return  -1;
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

    //获取手指按在屏幕上的两点距离
    private float spacing(MotionEvent event){
        float x = event.getX(0) - event.getX(1);
        float y = event.getY(0) - event.getY(1);
        return (float) (Math.sqrt(x*x+y*y));
    }

    float mPreviousX,mPreviousY;
    float mPreviousDir;
    boolean isDragging = false;
    boolean isZoomming = false;
    float newDir;
    static final float FOVY_MIN = 5.0f;
    static final float FOVY_MAX = 180.0f;
    Date touchDownTime;
    boolean hideOrShow;
    float angleYawVROffset = 0;
    public boolean onTouchEvent(MotionEvent event){
        float x = event.getX();
        float y = event.getY();
        //响应多点触摸操作
        switch (event.getAction()&MotionEvent.ACTION_MASK){
            case MotionEvent.ACTION_MOVE:
                if (isZoomming){
                    newDir = spacing(event);
                    float tScale =  (float)(Define.fovy - (Math.min(Math.max(newDir / mPreviousDir, 0.6), 1.4) - 1) * 10 * 2);
                    Define.fovy = Math.min(Math.max((tScale), FOVY_MIN), FOVY_MAX);
                    Log.d(TAG, "onTouchEvent: newDir/mPreviousDir：" + String.valueOf(newDir/mPreviousDir));
                    mPreviousDir = newDir;
                    break;
                }
                if (isDragging){
                    float dx = (x - mPreviousX)/10;
                    float dy = (y - mPreviousY)/10;
                    Define.x += dx;
                    Define.y += dy;
                    Log.d(TAG, "onTouchEvent: x:"+ Define.x + "，y:" + Define.y);
                    angleYawVROffset += dx;
                    mPreviousX = x;
                    mPreviousY = y;
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
                        if (hideOrShow){
                            layout.setVisibility(View.INVISIBLE);
                            hideOrShow = false;
                        }else {
                            layout.setVisibility(View.VISIBLE);
                            hideOrShow = true;
                        }
                    }
                }
                break;
            case MotionEvent.ACTION_POINTER_DOWN:
                touchDownTime= null;
                if (event.getPointerCount() == 2) {
                    isZoomming = true;
                    mPreviousDir = spacing(event);
                }
                break;
            case MotionEvent.ACTION_POINTER_UP:
                isDragging = false;
                isZoomming = false;
                touchDownTime= null;
                break;
        }
        return true;
}
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK&&event.getRepeatCount()==0){
//            for (int i=0;i<Define.cameraCount;i++){
                connectManager.stop();
//            }
            view = null;
            time.cancel();
            sensorManager.unregisterListener(this);
            decodeBuffer = null;
            frameBuffer = null;
            Define.mode = Define.MODE_GLOBULAR;
            Define.showVR = false;
            Define.fovy = 45;
            Define.x = 0;
            Define.y = 0;
            Define.z = 0;
            finish();
        }
        return true;
    }

    public void drawPoint(View view) {
        Define.pointCount++;
    }

    static int PITCH_MAX = 175;
    static int PITCH_MIN = 5;
    @Override
    public void onSensorChanged(SensorEvent event) {
        float dx = event.values[0];
        float dy = event.values[1];
        float dz = event.values[2];
        if (Define.showVR == true) {
            if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                float pitch = (float) Math.toDegrees(Math.atan(dx / dz));
                if (pitch < 0) {
                    pitch += 180;
                }
                pitch = Math.min(Math.max(pitch, PITCH_MIN), PITCH_MAX);
                float roll = (float) Math.toDegrees(Math.atan((dy - speedY) / dx));
                roll = Math.min(Math.max(roll, -85), 85);
                //非极端俯仰和横滚下
                if ((pitch != PITCH_MIN && pitch != PITCH_MAX) && (roll != -85 && roll != 85)) {
                    //参与辅助校准
                    radiansPitch = (radiansPitch * 3 + (float) Math.toRadians(pitch)) / 4;
                    //直接计算角度
                    angleRoll = (angleRoll * 3 + roll) / 4;
                }
            } else if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
                speedY = dy;
            }else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
                if (sensorTS != 0) {
                    double dT = (event.timestamp - sensorTS) / 1000000000.0f;
                    float radiansYawDifference = (float)(dx * dT);
                    float radiansPitchDifference = (float)(dy * dT);
                    radiansYaw += radiansYawDifference;
                    float angleXAim = (float) Math.toDegrees(radiansYaw);
                    radiansPitch -= radiansPitchDifference;
                    float angleYAim = (float) Math.toDegrees(radiansPitch);
                    angleYAim = Math.min(Math.max(angleYAim, PITCH_MIN), PITCH_MAX);
                    //滤波
                    angleYaw = (angleYaw * 3 + angleXAim) / 4;
                    anglePitch = (anglePitch * 3 + angleYAim) / 4;
                }
                sensorTS = event.timestamp;
            }
        }
        Define.x = angleYaw + angleYawVROffset;
        Define.y = anglePitch;
        //Log.d(TAG, "onSensorChanged: anglePitch"+anglePitch);
        Define.z = angleRoll;
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
    //pitch  俯仰角    yaw 偏航角     roll 滚转角
    float angleYaw = 0, anglePitch = 0, angleRoll = 0;
//    float angleYawVROffset = 0;
    float radiansYaw = 0, radiansPitch = 0;
    long sensorTS = 0;
    float speedX = 0,speedZ = 0,speedY = 0;

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
        Define.x = 0;
        Define.y = 0;
        Define.z = 0;
    }

    public void setVR(View view) {
        if (Define.showVR == false){
            Define.showVR = true;
            btnVR.setText("全景观看");
            showMode.setEnabled(false);
            sensorManager.registerListener(this, gyroscopeSensor, SensorManager.SENSOR_DELAY_GAME);
            sensorManager.registerListener(this, accelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
            sensorManager.registerListener(this, linearAccelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
        } else {
            Define.showVR = false;
            btnVR.setText("VR观看");
            showMode.setEnabled(true);
            sensorManager.unregisterListener(this, gyroscopeSensor);
            sensorManager.unregisterListener(this, accelerometerSensor);
            sensorManager.unregisterListener(this, linearAccelerometerSensor);
        }
        Define.x = 0;
        Define.y = 0;
        Define.z = 0;
    }

    public void setFlip(View view) {
        if (Define.flip == false){
            Define.flip = true;
        }else {
            Define.flip = false;
        }
        Define.isNeedFilp = true;
    }

    public void setColorTurn(View view) {
        if (Define.isCmbExist){
            if (!PEColorTune.isGainReady){
                getGain(Define.cameraCount);
            }
            if (PEColorTune.colorTurnFlag){
                PEColorTune.colorTurnFlag = false;
                initGain(Define.cameraCount);
                btnTurn.setText("修复色差");
            }else {
                PEColorTune.colorTurnFlag = true;
                setGain(Define.cameraCount);
                btnTurn.setText("重置色差");
            }
            for (int cid = 0;cid<Define.cameraCount;cid++)
                PEColorTune.updateGainList[cid] = 0;
        }else {
            Toast.makeText(this,"找不到cmb调色文件！",Toast.LENGTH_SHORT).show();
        }
    }
}
