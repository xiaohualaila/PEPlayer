
package panoeye.pelibrary;


import java.util.HashMap;

/**
 * Created by jun on 2017/12/12.
 */

 class PERPano extends Pano{
    //哈希表<键,值>
     HashMap<Integer,PERModule> files = new HashMap<>();//镜头id与per模块数据对照表
     PERFile perFile;
    //构造函数
     PERPano(String name)throws PEError {
        //文件完整路径
        final String path = String.format(Define.ROOT_PATH+name);
//        final String path = String.format(Define.ROOT_PATH+"E10010078_E1078_20170609_081834_0822.per");

         perFile = new PERFile(path);
         binFile = perFile.binFile;
         for (int cid:binFile.cidList) {
             //往哈希表files中添加数据（键值对）
             files.put(cid,perFile.PERModule[cid]);
         }
         minDuration = perFile.duration;
    }
     //初始化解码器
     HashMap<Integer,PEDecodeThread> initDecoders(PEDecodeBuffer decodeBuffer) throws PEError{
        HashMap<Integer,PEDecodeThread> thread = new HashMap<>();
         for (Integer cid:binFile.cidList){
            //获取主码流尺寸
            Size size = new Size(binFile.camConfigs.get(cid).fullSize.width,binFile.camConfigs.get(cid).fullSize.height);
            PEFrameBuffer buffer =(PEFrameBuffer) decodeBuffer.bufferDict.get(cid);//根据cid获取帧数据缓存类对象
            try {
                PEDecoder decoder = new PEDecoder(buffer,size);//构造解码器decoder
                PEDecodeThread decodeThread = new PEDecodeThread(decoder,cid);//解码线程
                thread.put(cid,decodeThread);//将解码线程添加到镜头-解码线程对照表
            }catch (PEError e){
                e.printStackTrace();
            }
        }
        return thread;
    }
}
