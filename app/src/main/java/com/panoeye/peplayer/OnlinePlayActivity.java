package com.panoeye.peplayer;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.TrafficStats;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.Button;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Timer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by tir on 2016/10/9.
 */
public class OnlinePlayActivity extends AppCompatActivity implements GLSurfaceView.Renderer, SensorEventListener {

    final WeakReference<OnlinePlayActivity> self = new WeakReference<OnlinePlayActivity>(this);
    //主子码流
    static final int STREAM_TYPE_MAIN = 0;
    static final int STREAM_TYPE_SUB = 1;
    static int PITCH_MAX = 175;
    static int PITCH_MIN = 5;
    static final float FOVY_MIN = 5.0f;
    static final float FOVY_MAX = 90.0f;
    static final int MODE_NORMAL = 0;
    static final int MODE_VR = 1;
    static final int MODE_EXITING = 2;
    static final int VIDEO_GLOBULAR = 0;
    static final int VIDEO_ORIGINAL = 1;

    boolean isFinish = false;
    boolean isFullScreen = false;
    private SensorManager sensorManager;
    private Sensor gyroscopeSensor;         //陀螺仪
    private Sensor accelerometerSensor;     //加速度
    private Sensor linearAccelerometerSensor;     //线加速度

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_onlineplay);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        setViews();
        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        gyroscopeSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        linearAccelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);

        hideCtrlViews();
    }

    float angleYaw = 0, anglePitch = 0, angleRoll = 0;
    float angleYawVROffset = 0;
    float radiansYaw = 0, radiansPitch = 0;
    long sensorTS = 0;
    float speedY = 0;
    @Override
    public void onSensorChanged(SensorEvent event) {
        float dx = event.values[0];
        float dy = event.values[1];
        float dz = event.values[2];
        if (mode == MODE_VR) {
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
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        DisplayMetrics metric = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metric);
        //长的是长，短的是宽
        int width = Math.max(metric.heightPixels, metric.widthPixels);
        int height = Math.min(metric.heightPixels, metric.widthPixels);
        JNIServerLib.glmInit(width, height);

    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        JNIServerLib.glmResize(i, i1);
    }

    int openglFrameRate = 0;
    @Override
    public void onDrawFrame(GL10 gl10) {
        if (mode == MODE_VR) {
            float x = angleYaw + angleYawVROffset;
            //陀螺仪
            JNIServerLib.glmStep(x, anglePitch, angleRoll, fovy);
            openglFrameRate++;
        } else if (mode == MODE_NORMAL) {
            JNIServerLib.glmStep(angleYaw, anglePitch, angleRoll, fovy);
            openglFrameRate++;
        } else {
            gl10.glClear(GL10.GL_COLOR_BUFFER_BIT);
        }
        if (isFinish && mode != MODE_EXITING) {
            mode = MODE_EXITING;
            sensorManager.unregisterListener(self.get());
            Intent intent = new Intent(self.get(), LoadingActivity.class);
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
            startActivity(intent);
            new Thread(new Runnable() {
                @Override
                public void run() {
                    while (LoadingActivity.loadingActivity == null) ;
                    JNIServerLib.unregistOnlinePanoCam();
                    mHandler.sendEmptyMessage(0);
                }
            }).start();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK){
            back(null);
        }
        return false;
    }

    float fovy = 45.0f;
    float mPreviousX, mPreviousY;
    float mPreviousDis;
    boolean isDragging = false;
    boolean isZoomming = false;
    Date touchDownTime;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_MOVE:
                if (isDragging) {
                    float dx = (x - mPreviousX) / 10;
                    float dy = (y - mPreviousY) / 10;
                    if (mode == MODE_VR) {
                        angleYawVROffset += dx;
                    } else {
                        angleYaw += dx;
                        anglePitch += dy;
                        anglePitch = Math.min(Math.max(anglePitch, PITCH_MIN), PITCH_MAX);
                    }
                    mPreviousX = x;
                    mPreviousY = y;
                }
                if (isZoomming) {
                    float distance = getDistance(event);
                    float tScale =  (float)(fovy - (Math.min(Math.max(distance / mPreviousDis, 0.6), 1.4) - 1) * 10 * 2);
                    fovy = Math.min(Math.max(tScale, FOVY_MIN), FOVY_MAX);
                    mPreviousDis = distance;
                }
                break;
            case MotionEvent.ACTION_DOWN:
                mPreviousX = x;
                mPreviousY = y;
                isDragging = true;
                touchDownTime = new Date();
                break;
            case MotionEvent.ACTION_POINTER_DOWN:
                if (event.getPointerCount() == 2) {
                    isZoomming = true;
                    mPreviousDis = getDistance(event);
                }
                touchDownTime = null;
                isDragging = false;
                break;
            case MotionEvent.ACTION_UP:
                isDragging = false;
                isZoomming = false;
                if (touchDownTime != null) {
                    Date nowTime = new Date();
                    if (nowTime.getTime() - touchDownTime.getTime() <= 100) {
                        if (isFullScreen == true) {
                            showCrtlViews();
                        } else {
                            hideCtrlViews();
                        }
                    }
                }
                break;
            case MotionEvent.ACTION_POINTER_UP:
                isDragging = false;
                isZoomming = false;
                touchDownTime = null;
                break;
            case MotionEvent.ACTION_CANCEL:
                isDragging = false;
                isZoomming = false;
                touchDownTime = null;
                break;
        }
        return true;
    }
    public void back(View btn) {
        new AlertDialog.Builder(this)
                .setTitle("退出播放")
                .setMessage("确定要退出播放吗?")
                .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        frameRateTimer.cancel();
                        isFinish = true;
                        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
                    }
                })
                .setNegativeButton("取消", null)
                .setCancelable(false)
                .show();
    }


    int mode = MODE_NORMAL;
    public void mode(View btn) {
        if (videoMode == VIDEO_ORIGINAL) {
            new AlertDialog.Builder(this)
                    .setTitle("提示")
                    .setMessage("播放原始图像时VR模式不可用！")
                    .setPositiveButton("确定", null)
                    .setCancelable(false)
                    .show();
            return;
        }
        Log.d("SHOWVR", ""+mode);
        if (mode == MODE_NORMAL) {
            mode = MODE_VR;
            modeBtn.setText("正常");
            angleYawVROffset = 0;
            angleYaw = 0;
            radiansYaw = 0;
            sensorTS = 0;
            sensorManager.registerListener(this, gyroscopeSensor, SensorManager.SENSOR_DELAY_GAME);
            sensorManager.registerListener(this, accelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
            sensorManager.registerListener(this, linearAccelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
        } else if (mode == MODE_VR) {
            mode = MODE_NORMAL;
            modeBtn.setText("VR");
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
            sensorManager.unregisterListener(this, gyroscopeSensor);
            sensorManager.unregisterListener(this, accelerometerSensor);
            sensorManager.unregisterListener(this, linearAccelerometerSensor);
            angleRoll = 0;
        }

        JNIServerLib.setMode(mode);
    }

    public void flip(View btn)
    {
        if (videoMode == VIDEO_ORIGINAL) {
            new AlertDialog.Builder(this)
                    .setTitle("提示")
                    .setMessage("播放原始图像时上下翻转不可用！")
                    .setPositiveButton("确定", null)
                    .setCancelable(false)
                    .show();
            return;
        }
        JNIServerLib.flip();
    }

    int streamType = STREAM_TYPE_MAIN;
    public void streamType(View btn) {
        Intent intent = new Intent(self.get(), LoadingActivity.class);
        startActivity(intent);
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (LoadingActivity.loadingActivity == null) ;
                if (streamType == STREAM_TYPE_MAIN) {
                    streamType = STREAM_TYPE_SUB;
                } else if (streamType == STREAM_TYPE_SUB) {
                    streamType = STREAM_TYPE_MAIN;
                }
                if (JNIServerLib.setStreamType(streamType) == false) {
                    mHandler.sendEmptyMessage(3);
                } else {
                    mHandler.sendEmptyMessage(4);
                }
            }
        }).start();

    }

    int videoMode = VIDEO_GLOBULAR;
    public void videoMode(View btn) {
        if (mode == MODE_VR) {
            mode(null);
        }
        if (videoMode == VIDEO_GLOBULAR) {
            videoMode = VIDEO_ORIGINAL;
            videoModeBtn.setText("全景图像");
            PITCH_MAX = 90;
            PITCH_MIN = -90;
            angleYaw = 0;
            anglePitch = 0;
            angleRoll = 0;
        } else {
            videoMode = VIDEO_GLOBULAR;
            videoModeBtn.setText("原始图像");
            PITCH_MAX = 175;
            PITCH_MIN = 5;
            angleYaw = 0;
            anglePitch = PITCH_MIN;
            angleRoll = 0;
        }
        JNIServerLib.setVideoMode(videoMode);
    }

    private void showCrtlViews() {
        AlphaAnimation animation = new AlphaAnimation(0.0f, 0.8f);
        animation.setDuration(200);
        animation.setFillAfter(true);
        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                topView.setAlpha(0.8f);
                btnView.setAlpha(0.8f);
                videoFrameRateLabel.setAlpha(0.8f);
                glFrameRateLabel.setAlpha(0.8f);
                netLabel.setAlpha(0.8f);
                cpuLabel.setAlpha(0.8f);
                timeLabel.setAlpha(0.8f);
            }

            @Override
            public void onAnimationEnd(Animation animation) {
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });
        topView.startAnimation(animation);
        btnView.startAnimation(animation);
        videoFrameRateLabel.startAnimation(animation);
        glFrameRateLabel.startAnimation(animation);
        netLabel.startAnimation(animation);
        cpuLabel.startAnimation(animation);
        timeLabel.startAnimation(animation);
        findViewById(R.id.backBtn).setVisibility(View.VISIBLE);
        findViewById(R.id.modeBtn).setVisibility(View.VISIBLE);
        findViewById(R.id.flipBtn).setVisibility(View.VISIBLE);
        findViewById(R.id.streamTypeBtn).setVisibility(View.VISIBLE);
        findViewById(R.id.videoModeBtn).setVisibility(View.VISIBLE);
        isFullScreen = false;
    }

    private void hideCtrlViews() {
        AlphaAnimation animation = new AlphaAnimation(0.8f, 0.0f);
        animation.setDuration(200);
        animation.setFillAfter(true);
        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                topView.setAlpha(0.0f);
                btnView.setAlpha(0.0f);
                videoFrameRateLabel.setAlpha(0.0f);
                glFrameRateLabel.setAlpha(0.0f);
                netLabel.setAlpha(0.0f);
                cpuLabel.setAlpha(0.0f);
                timeLabel.setAlpha(0.0f);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });
        topView.startAnimation(animation);
        btnView.startAnimation(animation);
        videoFrameRateLabel.startAnimation(animation);
        glFrameRateLabel.startAnimation(animation);
        netLabel.startAnimation(animation);
        cpuLabel.startAnimation(animation);
        timeLabel.startAnimation(animation);
        findViewById(R.id.backBtn).setVisibility(View.GONE);
        findViewById(R.id.modeBtn).setVisibility(View.GONE);
        findViewById(R.id.flipBtn).setVisibility(View.GONE);
        findViewById(R.id.streamTypeBtn).setVisibility(View.GONE);
        findViewById(R.id.videoModeBtn).setVisibility(View.GONE);
        isFullScreen = true;
    }

    public float getDistance(MotionEvent Event)	{
        float deltalX = Event.getX(0) - Event.getX(1);
        float deltalY = Event.getY(0) - Event.getY(1);
        return (float) Math.sqrt(deltalX * deltalX + deltalY * deltalY);
    }

    Timer frameRateTimer;
    GLSurfaceView mView;
    Button modeBtn, streamTypeBtn, videoModeBtn;
    View topView, btnView;
    TextView textView;
    TextView videoFrameRateLabel, glFrameRateLabel, netLabel, cpuLabel, timeLabel;
    private void setViews() {
        topView = (View) findViewById(R.id.topView);
        btnView = (View) findViewById(R.id.btnView);
        modeBtn = (Button)findViewById(R.id.modeBtn);
        streamTypeBtn = (Button)findViewById(R.id.streamTypeBtn);
        videoModeBtn = (Button)findViewById(R.id.videoModeBtn);
        textView = (TextView)findViewById(R.id.titleTextView);
        mView = (GLSurfaceView) findViewById(R.id.surfaceView);
        videoFrameRateLabel = (TextView) findViewById(R.id.videoFrameRateLabel);
        glFrameRateLabel = (TextView) findViewById(R.id.glFrameRateLabel);
        netLabel = (TextView) findViewById(R.id.netLabel);
        cpuLabel = (TextView) findViewById(R.id.cpuLabel);
        timeLabel = (TextView) findViewById(R.id.timeLabel);
        String name = (String)getIntent().getSerializableExtra("NAME");
        textView.setText(name);

        mView.setEGLContextClientVersion(2);
        mView.setRenderer(this);

//        frameRateTimer = new Timer();
//        TimerTask task = new TimerTask(){
//            public void run() {
//                mHandler.sendEmptyMessage(2);
//            }
//        };
//        frameRateTimer.schedule(task, 0, 1000);
    }
    final private Handler mHandler = new Handler(){
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 0:
                    LoadingActivity.loadingActivity.close();
                    self.get().finish();
                    break;
                case 1:
                    break;
                case 2://每秒刷新帧率
                    glFrameRateLabel.setText("OpenGL: " + String.valueOf(openglFrameRate) + " fps");
                    videoFrameRateLabel.setText("Videos: " + String.valueOf(JNIServerLib.getDecodeFrameRate()) + " fps");
                    openglFrameRate = 0;
                    netLabel.setText("Network: " + String.valueOf(getAppNetworkSpeed()) + " kBps");
                    cpuLabel.setText("CPU: " + String.valueOf(((int)(getAppCpuRate() * 100)) / 100.0) + " %");
                    long ts = JNIServerLib.getOnlinePanoCurrentTimeStamp();
                    Date date = new Date(ts);
                    SimpleDateFormat sf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    timeLabel.setText(sf.format(date));
                    break;
                case 3:
                    LoadingActivity.loadingActivity.close();
                    new AlertDialog.Builder(self.get())
                            .setTitle("提示")
                            .setMessage(JNIServerLib.getLastErrorMessage())
                            .setPositiveButton("确定", null)
                            .setCancelable(false)
                            .show();
                    break;
                case 4:
                    LoadingActivity.loadingActivity.close();
                    if (streamType == STREAM_TYPE_MAIN) {
                        streamTypeBtn.setText("子码流");
                    } else if (streamType == STREAM_TYPE_SUB) {
                        streamTypeBtn.setText("主码流");
                    }
                    break;
                case 5:
                    mode(null);
                default :
                    break;
            }
        }
    };

    private long lastTotalRxBytes = 0;
    private long lastTimeStamp = 0;

    private int getAppNetworkSpeed() {
        long nowTotalRxBytes = getTotalRxBytes();
        long nowTimeStamp = System.currentTimeMillis();
        long speed = ((nowTotalRxBytes - lastTotalRxBytes) * 1000 / (nowTimeStamp - lastTimeStamp));//毫秒转换
        lastTimeStamp = nowTimeStamp;
        lastTotalRxBytes = nowTotalRxBytes;
        return (int)speed;
    }

    private long getTotalRxBytes() {
        return TrafficStats.getUidRxBytes(getApplicationInfo().uid)==TrafficStats.UNSUPPORTED ? 0 :(TrafficStats.getTotalRxBytes()/1024);//转为KB
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

}
