package com.panoeye.peplayer.peonline;
import android.util.Log;

import java.nio.ByteBuffer;
import java.util.HashMap;

import panoeye.pelibrary.Define;
import panoeye.pelibrary.PEDecodeBuffer;
import panoeye.pelibrary.PEDecoder;
import panoeye.pelibrary.PEError;
import panoeye.pelibrary.PEFrameBuffer;
import panoeye.pelibrary.Size;
import java.io.RandomAccessFile;
/**
 * Created by Administrator on 2018/2/9.
 */

public class ConnectManager {
    PEDecoder[] decoder = new PEDecoder[Define.cameraCount];
//    static FEDrawManager[] drawManagers = new FEDrawManager[Define.cameraCount];
    private static final String TAG = "ConnectManager";
    boolean isRun = true;
    Integer connSuccessCount = 0;
    public PEDecodeBuffer decodeBuffer;
    RandomAccessFile fp;
    //记录相机连接状态
//    public final List<String> flags = Collections.synchronizedList(new ArrayList<String>());
//    public String[] connFlags  = new String[Define.cameraCount];
    public int connErrCnt = 0;
    public HashMap flagsDict = new HashMap();

    public int InitRTSP(String addr,int handle){
        Log.e(TAG, "Connect:"+handle);


        return RTSPConnect(addr,handle);
    }
    public void closeRTSP(){
        for (int cid=0;cid<Define.cameraCount;cid++){
            if (flagsDict.get(cid)!= null){
                closeRTSP(cid);
                Log.e(TAG, "closeRTSP:"+cid);
            }
        }
    }
//    public static int InitRTSP(final String[] addrs){
//        for (int cid=0;cid<Define.cameraCount;cid++){
//            final int finalHandle = cid;
//            new Thread(new Runnable(){
//                @Override
//                public void run() {
////                    synchronized(connSuccessCount){
//                        Log.e(TAG, "Connect:"+finalHandle);
//                        int ret = RTSPConnect(addrs[finalHandle],finalHandle);
//                        if (ret != -1){
//                            flags.add("Succeed");
//                            connSuccessCount++;
//                        }else {
//                            flags.add("Failed");
//                            Log.e(TAG,"模组"+ finalHandle +"-->RTSPConnect()失败!");
//                        }
////                    }
//                }
//            }).start();
//        }
//        int sleepCount = 0;
//        while (true){
//            Log.d(TAG, "onCreate: true循环，连接相机中...");
//            Log.d(TAG, "onCreate: flags.size()-->:"+flags.size());
//            if (flags.size() == Define.cameraCount){
//                Log.d(TAG, "onCreate: true循环，连接相机完成！");
//                break;
//            }
//            Thread.yield();
//            try {
//                sleepCount++;
//                Log.d(TAG, "onCreate: sleepCount:"+sleepCount);
//                if (sleepCount>=5){
//                    return -1;
//                }
//                Thread.sleep(2000);//线程休眠2s
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//        }
//        return connSuccessCount;
//    }

    public boolean InitDecoder(){
            int width = getWidth(0);   //1280
            int heigth = getHeight(0); //960
            Size size = new Size(width, heigth);
            Log.i(TAG, "InitDecoder:size " + size.width + "," + size.height);
            decodeBuffer = new PEDecodeBuffer(Define.cameraCount, size);
            try {
                for (int cid=0;cid<Define.cameraCount;cid++) {
                    PEFrameBuffer frameBuffer = (PEFrameBuffer) decodeBuffer.bufferDict.get(cid);//根据cid获取帧数据缓存类对象
                    decoder[cid] = new PEDecoder(frameBuffer, size);
                    Log.e(TAG, "decoder" + cid + ":Create!");
                }
            } catch (PEError peError) {
                peError.printStackTrace();
            }
        return true;
    }
    public void runLoop(int finalHandle) {
                    while (true){
                        try {
                            ByteBuffer buffer = getPacket(finalHandle);
                            if (buffer!=null){
                                decoder[finalHandle].decode(buffer, finalHandle);
                            }
            //                        if (handle == 0){
            //                            Define.video1 +=1;
            //                        }
            //                        if (handle == 1){
            //                            Define.video2 +=1;
            //                        }
                            clearPacket(finalHandle);
                                    try {
                                        Thread.sleep(20);//线程休眠20ms
                                    } catch (InterruptedException e) {
                                        e.printStackTrace();
                                    }
                        } catch (PEError peError) {
                            peError.printStackTrace();
                        }
                        if (isRun == false){
                            clear(finalHandle);
                            decoder[finalHandle].stop();
                            Log.e(TAG, "stop:"+ finalHandle);
                            break;
                        }
                    }
//                }
//            }).start();
//        }
    }
    public void stop(){
        isRun = false;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

//    public native static int getHandle();
    public native static int RTSPConnect(String input,int handle);
    public native static int getWidth(int handle);
    public native static int getHeight(int handle);
    public native static ByteBuffer getPacket(int handle);
    public native static void clearPacket(int handle);
    public native static void clear(int handle);
    public native static void closeRTSP(int handle);

}
