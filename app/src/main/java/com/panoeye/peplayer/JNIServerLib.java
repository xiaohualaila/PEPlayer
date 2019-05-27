package com.panoeye.peplayer;

import android.view.Surface;

/**
 * Created by tir on 2016/8/31.
 */
public class JNIServerLib {
    static {
        System.loadLibrary("JNIServerLib");
    }

    public static native String getLastErrorMessage();

    public static native void glmInit(int width, int height);
    public static native void glmResize(int width, int height);
    public static native void glmStep(float gldx, float gldy, float gldz, float fovy);

    //
    public static native boolean registPano(String filename, int panoType);
    public static native void unregistPano();
    public static native void setTimeline(float timeline);
    public static native boolean awakeDecoders();
    public static native boolean runPano();
    public static native float jumpPanoToTimeline();
    public static native boolean resumePano();
    public static native boolean pausePano();
    public static native float getTotalDuration();
    public static native int getDecodeFrameRate();
    public static native void setMode(int mode);
    public static native void flip();
    public static native boolean setStreamType(int type);
    public static native void setVideoMode(int mode);

    //
    public static native boolean registOnlinePano(String ip, int port, String username, String password);
    public static native int getOnlinePanoCamCount();
    public static native byte[] getOnlinePanoCamNameWithIndex(int index);
    public static native boolean registOnlinePanoCamWithIndex(int index);
    public static native void unregistOnlinePanoCam();
    public static native long getOnlinePanoCurrentTimeStamp();
    public static native int getOnlinePanoFileDownloadingPos();
}
