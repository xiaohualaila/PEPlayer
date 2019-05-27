package panoeye.pelibrary;

import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Created by tir on 2016/12/14.
 * 帧数据缓存类
 */

 public class PEFrameBuffer {
    private static final String TAG = "PEFrameBuffer";
    Integer cid;//镜头id
     private LockWrap<ByteBuffer> _imgBuffer = new LockWrap<>(ByteBuffer.allocate(0)) ;//图像缓存
     public Size size;//主码流尺寸
     Integer bufferSize = 0;//一帧画面的缓存大小
     public boolean isNeedReload = false;//标志是否有解码好的数据
    //设置ByteBuffer类对象
     public void set(ByteBuffer byteBuffer){
        _imgBuffer.set(byteBuffer);//将byteBuffer设置为_imgBuffer的内容
    }
    //获取ByteBuffer类对象
    ByteBuffer get(){
        return _imgBuffer.get();//通过_imgBuffer获取到byteBuffer
    }
     boolean tryLock(){
        return _imgBuffer.tryLock();
    }
    //ByteBuffer类对象上锁
     void lock(){
        _imgBuffer.lock();
    }
    //ByteBuffer类对象解锁
     void unlock(){
        _imgBuffer.unlock();
    }

     PEFrameBuffer(int cameraId,Size size){
         this.cid = cameraId;
         bufferSize = size.width*size.height*3;//3686400=1280*960*3
         this.size = size;
         Log.d(TAG, "PEFrameBuffer1: cid:"+cid);
         Log.d(TAG, "PEFrameBuffer1: width:"+size.width+",height:"+size.height);
     }
     //构造函数
     public PEFrameBuffer(PECameraConfig config){
        this.cid = config.cid;
        bufferSize = config.fullSize.height*config.fullSize.width*3;//获取一帧画面的缓存大小//9437184=2048*1536*3
        this.size = config.fullSize;
         Log.d(TAG, "PEFrameBuffer2: cid:"+cid+",bufferSize"+bufferSize);
         Log.d(TAG, "PEFrameBuffer2: size:"+size.width+","+size.height);
    }

}
