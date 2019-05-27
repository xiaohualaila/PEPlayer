package panoeye.pelibrary;

import android.util.Log;


/**
 * Created by tir on 2016/12/15.
 */
//解码线程类
 class  PESDecodeThread extends Thread{
    private PEDecoder decoder;//解码器
    private PESFile pesFile;//pes文件
    private Integer index = 0;//
    private Thread thread;//线程
    private Integer cid;//镜头id
    private boolean needDecode;//
    private boolean stopFlag = true;//
    boolean threadStop = false;//
    float tmp = 0f;//
    //构造函数
     PESDecodeThread(PEDecoder decoder, PESFile pesFile,Integer cid ){
        this.decoder = decoder;
        this.pesFile = pesFile;
        this.cid = cid;
    }
    //重写了父类的run方法
    public void run() {
/*        while (true){
            try {
                if(needDecode){
                    needDecode = false;
                    decoder.decode(pesFile.getPEVideoPacket(index));
                    //Log.e(String.format("线程%d",cid),String.format("%d",index));
                }else{
                    Thread.sleep(20);
                }
            }catch (InterruptedException e){
                e.printStackTrace();
            }
        }*/
        synchronized (cid){
            try {
                while (true){
                    if (stopFlag==false){
                        break;
                    }
                    if(needDecode == true) {
                        if (index<pesFile.timestampList.size()-1){
                            decoder.decode(pesFile.getPESVideoPacket(index -1).oneFrameData,cid,1);//解码
                        }
                        needDecode = false;
                        cid.wait();
                    }
                }
                Log.i("线程已经结束！","msg");
                threadStop = true;
            }catch (InterruptedException e){
                Log.e("线程","报错");
            }catch (PEError e){
                Log.e("线程","报错了");
            }
        }
    }
    void threadStop(){
        stopFlag = false;
    }

    void jumpToTime(Float jumpToTime) {
        if (jumpToTime == 0){
            index = 0;
            return;
        }
        if (jumpToTime >= 0f && jumpToTime+pesFile.firstPacketTimestampOffset <= pesFile.IFrame.get(pesFile.IFrame.size()-1)) {
            int i = 0;
            while (true){
                if ((jumpToTime+pesFile.firstPacketTimestampOffset)<=pesFile.IFrame.get(i)){
                    tmp = pesFile.IFrame.get(i-1);
                    break;
                }
                i++;
            }
            i = 0;
            while (true){
                if (tmp == pesFile.timestampList.get(i)){
                    index = i;
                    if (index == 0){
                        index =1;
                    }
                    tmp =pesFile.timestampList.get(i)-pesFile.firstPacketTimestampOffset;
                    break;
                }
                i++;
            }
        }
    }
     //时间轴控制
     void step(Float timeLine ){
/*        if(thread == null){
            thread = new Thread(this);
            thread.start();
        }
        if(timeLine>=(pesFile.timestampList.get(index))-pesFile.firstPacketTimestampOffset){
            index+=1;
            needDecode = true;
        }*/


        if(thread == null){
            thread = new Thread(this);
            thread.start();
        }
        synchronized (cid){//synchronized() 创建了一个互斥锁,防止｛｝里的内容在同一时间内被其他线程访问，起到了线程保护的作用
            if (index<pesFile.timestampList.size()-1) {
                if (timeLine >= (pesFile.timestampList.get(index)) - pesFile.firstPacketTimestampOffset) {
                    index += 1;//index自增1
                    needDecode = true;//需要解码
                    cid.notify();//唤醒线程
                    //Log.d("index", index.toString());
                    //Log.e(String.format("线程%d",cid),String.format("%d",index-1));
                }
            }
        }

    }

}
