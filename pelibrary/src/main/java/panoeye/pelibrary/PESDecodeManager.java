package panoeye.pelibrary;



import android.util.Log;

import java.util.HashMap;

/**
 * Created by tir on 2016/12/13.
 */

 public class PESDecodeManager {
    private Float TIMELINE_STEP = 0.02f;//时间轴每次增长的时间
    boolean pauseFlag = true;//暂停标志
    Thread thread;//线程
    HashMap<Integer,PESDecodeThread> threads = new HashMap<>();//镜头与解码线程对照表
    LockWrap<Float> timeline = new LockWrap<Float>(0.0f);//时间轴
    //public LockWrap<HashMap<Integer,Integer>> frameCounter = new  LockWrap<>(new HashMap<Integer,Integer>());
    LockWrap<PEDecodeManagerStatus> status = new LockWrap<>(PEDecodeManagerStatus.over);//解码管理状态
//    PESDecodeManagerConfig config;//解码管理配置
    PESPano pano;
    PEDecodeBuffer decodeBuffer;
     //构造函数
     PESDecodeManager(PESPano pano,PEDecodeBuffer decodeBuffer){
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


    void invalidate() throws PEError{
        if (status.get()==PEDecodeManagerStatus.over){
            timeline.set(0f);
            setJumpToTime(0f);
        }
        pauseFlag = false;
        for(Integer cid:pano.binFile.cidList){
            threads.get(cid).threadStop();
        }
        for(Integer cid:pano.binFile.cidList){
            while (true){
                if (threads.get(cid).threadStop==true){
                    Log.i(String.valueOf(cid),"结束");
                    break;
                }
            }
        }
        status.set(PEDecodeManagerStatus.stop);
    }
    //开始解码绘制
    void start() throws PEError{
        initDecoders();//解码初始化器
        pauseFlag = false;
        timeline.set(0f);//设置时间轴初始值
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
        for(Integer cid:pano.binFile.cidList){
            threads.get(cid).jumpToTime(0f);
        }
        status.set(PEDecodeManagerStatus.play);
    }
    boolean setJumpToTime(Float jumpToTime){
        if (jumpToTime>pano.minDuration){
            Log.d(jumpToTime.toString(),jumpToTime.toString());
            return false;
        }
        for(Integer cid:pano.binFile.cidList){
            threads.get(cid).jumpToTime(jumpToTime);
        }
        if (jumpToTime>0) {
            float timetmp = threads.get(0).tmp;

            for (int i = 1; i < threads.size(); i++) {
                if (threads.get(i).tmp < timetmp) {
                    timetmp = threads.get(i).tmp;
                }
            }
            timeline.set(timetmp);
            return true;
        }
        timeline.set(jumpToTime);
        return true;
    }
     //run()的循环体
     void runloop()  throws InterruptedException,PEError{
        while (true){
            if(pauseFlag == true){
                Thread.sleep(200);//线程休眠200ms
                continue;
            }
            if (status.get()==PEDecodeManagerStatus.stop){//如果播放状态停止
                break;
            }
            //依次执行解码线程
            for(Integer cid:pano.binFile.cidList){
//            for (Integer cid=0;cid<1;cid++){//只解码7个通道
                threads.get(cid).step(timeline.get());//执行step()函数
            }
//            threads.get(0).step(timeline.get());//执行step()函数
//            threads.get(6).step(timeline.get());//执行step()函数
//            threads.get(5).step(timeline.get());//执行step()函数
//            threads.get(4).step(timeline.get());//执行step()函数
//            threads.get(3).step(timeline.get());//执行step()函数
//            threads.get(2).step(timeline.get());//执行step()函数
//            threads.get(1).step(timeline.get());//执行step()函数

            if (timeline.get()>pano.minDuration){//播放到了文件末尾
                status.set(PEDecodeManagerStatus.over);//设置播放状态结束
            }
            else {
                timeline.set(timeline.get()+TIMELINE_STEP);//设置时间轴增长步长
            }
             Thread.sleep(20);//线程休眠20ms

            //Log.d("runloop: ",timeline.get().toString() );
        }
    }


}
