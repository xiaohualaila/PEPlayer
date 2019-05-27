package panoeye.pelibrary;



import android.util.Log;

import java.util.HashMap;

/**
 * Created by jun on 2017/12/13.
 */

 public class PEDecodeManager {
    private static final String TAG = "PEDecodeManager";
    private Float TIMELINE_STEP = 0.07f;//时间轴每次增长的时间
    boolean pauseFlag = true;//暂停标志
    Thread thread;//线程
    HashMap<Integer,PEDecodeThread> threads = new HashMap<>();//镜头与解码线程对照表
    LockWrap<Float> timeline = new LockWrap<Float>(0.0f);//时间轴
    //public LockWrap<HashMap<Integer,Integer>> frameCounter = new  LockWrap<>(new HashMap<Integer,Integer>());
    LockWrap<PEDecodeManagerStatus> status = new LockWrap<>(PEDecodeManagerStatus.over);//解码管理状态
    PERPano pano;
    PEDecodeBuffer decodeBuffer;
     //构造函数
     PEDecodeManager(PERPano pano,PEDecodeBuffer decodeBuffer){
        this.pano = pano;
        this.decodeBuffer = decodeBuffer;
    }
     //初始化解码器
     void initDecoders(){
        try {
            threads = pano.initDecoders(decodeBuffer);
        }catch (PEError e){
            e.printStackTrace();
        }
    }
    //结束播放
    void invalidate() throws PEError{
        status.set(PEDecodeManagerStatus.stop);
    }
    //开始解码绘制
    void start() throws PEError{
        initDecoders();//解码初始化器
        pauseFlag = false;
        timeline.set(0f);//设置时间轴初始值//97f
        //创建线程
        thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    runloop();//run()的循环体
                } catch (InterruptedException E) {
                    PEError.gen(PEErrorType.none, "休眠失败!");
                }catch (PEError error){

                }
                Log.i("runloop","结束");
            }
        });
        thread.start();//启动线程
        status.set(PEDecodeManagerStatus.play);
    }

    void reView(){
        pauseFlag = false;
        status.set(PEDecodeManagerStatus.play);
    }

    void pause()  {
        pauseFlag = true;
        status.set(PEDecodeManagerStatus.pause);
    }
    void restart(){
        timeline.set(0f);
        setJumpToTime(0f);
        status.set(PEDecodeManagerStatus.play);
    }
    boolean setJumpToTime(float jumpToTime){
        if (jumpToTime>pano.minDuration){
            Log.d(TAG,"setJumpToTime:"+jumpToTime);
            return false;
        }
        float timeStamp = 0.0f;
        int timeStampIndex = 0;
        int fpPosition;
        if (jumpToTime>0) {
            timeStampIndex = (int)((jumpToTime/pano.minDuration)*pano.perFile.mainStreamIndexCount);
        }
        if (timeStampIndex<pano.perFile.mainStreamIndexCount){
            timeStamp = pano.perFile.iFrameTimestampList.get(timeStampIndex);
            fpPosition = pano.perFile.iFrameTimestampDict.get(timeStamp);
            pano.perFile.currentFramePosition = fpPosition;
            timeline.set(timeStamp);
            return true;
        }
        return false;
    }
     //run()的循环体
     void runloop()  throws InterruptedException,PEError{
        int camCount = pano.binFile.info.camCount;
        while (true){
            if(pauseFlag == true){//如果播放状态暂停
                Thread.sleep(200);//线程休眠200ms
                continue;//跳过下面的语句，重新执行runloop()函数
            }
            if (status.get()==PEDecodeManagerStatus.stop){//如果播放状态停止
                break;
            }
            if (timeline.get()>pano.minDuration){//播放到了文件末尾
                status.set(PEDecodeManagerStatus.over);//设置播放状态结束
                Thread.sleep(200);//线程休眠200ms
                continue;
            }else {
                timeline.set(timeline.get()+TIMELINE_STEP);//设置时间轴增长步长
            }
            //Log.d("runloop: ",timeline.get().toString() );
            for (int i=0;i<camCount;i++) {
                int cid = pano.perFile.findOneFrame(pano.perFile.fp);
                if (cid!=-1) {
//                    Log.d(TAG, "runloop: cid:"+cid);
                    threads.get(cid).step(timeline.get(), pano.perFile.oneFrameData);//执行step()函数
                }
            }
            Thread.sleep(40);//线程休眠20ms
        }
    }


}
