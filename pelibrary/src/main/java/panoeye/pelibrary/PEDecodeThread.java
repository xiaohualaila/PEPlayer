package panoeye.pelibrary;

import java.nio.ByteBuffer;


/**
 * Created by tir on 2016/12/15.
 */
//解码线程类
 class  PEDecodeThread extends Thread{
    private PEDecoder decoder;//解码器
    private Integer index = 0;//1400
    public Integer cid;//镜头id
    //构造函数
     PEDecodeThread(PEDecoder decoder,Integer cid ){
        this.decoder = decoder;
        this.cid = cid;
    }
     //时间轴控制
    void step(Float timeLine,ByteBuffer oneFrame) throws PEError {
//        synchronized (cid){//synchronized() 创建了一个互斥锁,防止｛｝里的内容在同一时间内被其他线程访问，起到了线程保护的作用
//            Log.d(String.format("线程%d",cid),String.format("时间轴%f",timeLine)+String.format("，帧序号%d",index));
            if (oneFrame != null) {
//                decoder.decode(oneFrame);//解码一帧数据
                decoder.decode(oneFrame,cid);//解码一帧数据
//                decoder.decode(oneFrame,cid,index);//解码一帧数据
            }
            index += 1;//index自增1
//        }
    }
}
