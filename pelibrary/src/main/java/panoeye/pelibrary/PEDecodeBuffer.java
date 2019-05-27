package panoeye.pelibrary;


import java.util.HashMap;
import java.util.Map;

/**
 * Created by tir on 2016/12/14.
 */
//解码缓存类
 public class PEDecodeBuffer  {
    private static final String TAG = "PEDecodeBuffer";
    //解码缓存映射表
    public MapThrowWrap bufferDict = new MapThrowWrap(new LockWrap<Map<Integer,PEFrameBuffer>>(new HashMap<Integer,PEFrameBuffer>()));
    //纹理数据映射表
    public MapThrowWrap textureDict = new MapThrowWrap(new LockWrap<Map<Integer,PEVideoTexture>>(new HashMap<Integer,PEVideoTexture>()));

//    //构造函数：鱼眼相机解码缓存
//    public PEDecodeBuffer(int cameraCount){
//        Size size;
//        if (cameraCount == 1){
//            size = new Size(960,960);
//        }else {
//            size = new Size(1280,960);
//        }
//        for (int cid=0;cid<cameraCount;cid++){
//            PEFrameBuffer frameBuffer = new PEFrameBuffer(cid,size);
//            bufferDict.put(cid,frameBuffer);//添加帧数据缓存类对象到缓存映射表，以后可根据cid得到对应cid的frameBuffer配置
//        }
//    }
    //构造函数：普通相机解码缓存
    public PEDecodeBuffer(int cameraCount,Size size){
//        Size size = new Size(1280,960);
        for (int cid=0;cid<cameraCount;cid++){
            PEFrameBuffer frameBuffer = new PEFrameBuffer(cid,size);
            bufferDict.put(cid,frameBuffer);//添加帧数据缓存类对象到缓存映射表，以后可根据cid得到对应cid的frameBuffer配置
        }
    }
    //构造函数：全景相机解码缓存
    public PEDecodeBuffer(PEBinFile binFile)throws PEError{
        for(Integer cid:binFile.cidList){
            //binFile.camConfigs.get(cid)方法 返回PECameraConfig类对象
            if(binFile.camConfigs.get(cid) == null){
                throw PEError.gen(PEErrorType.fileReadFailed,String.format("未找到相机配置！cid:%02d",cid));
            }
            //通过传入相机配置类对象构造帧数据缓存类
            PEFrameBuffer frameBuffer = new PEFrameBuffer(binFile.camConfigs.get(cid));
            bufferDict.put(cid,frameBuffer);//添加帧数据缓存类对象到缓存映射表，以后可根据cid得到对应cid的frameBuffer配置
        }
    }
    //更新纹理数据映射表
    public void undateIfNeed(Integer cid, int[] textures) throws PEError{
         //根据cid从缓存映射表中取出缓存数据
        PEFrameBuffer frameBuffer = (PEFrameBuffer)bufferDict.get(cid);
        if(frameBuffer.isNeedReload == true){//如果有解码好的数据就取出 到全景视频纹理类对象texture
            PEVideoTexture texture = new PEVideoTexture(frameBuffer,textures);
            textureDict.put(cid,texture);
            frameBuffer.isNeedReload = false;//取出完毕，
//            Log.d(TAG, "undateIfNeed取出完毕:"+cid);
        }
    }

}
