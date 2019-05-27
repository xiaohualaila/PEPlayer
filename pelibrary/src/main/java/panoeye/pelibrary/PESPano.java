
package panoeye.pelibrary;


import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;

/**
 * Created by tir on 2016/12/12.
 */

 class PESPano extends Pano{
// class PESPano{
    //哈希表<键,值>
     HashMap<Integer,PESFile> files = new HashMap<>();//镜头与pes录像数据对照表
//     PEBinFile binFile ;//bin文件
//     Float minDuration = 0f;//pse文件第一个I帧到最后一帧之间的最小时间间隔，单位为秒。
    //构造函数
     PESPano(String name)throws PEError {
    //记录文件读取状态
    final List<String> flags = Collections.synchronizedList(new ArrayList<String>());
    //读取bin文件
    binFile = new PEBinFile(Define.ROOT_PATH+name+"/"+name+".bin");
    //依次读取每个pes视频文件//0/6/5/4/3/2/1/7
    for(Integer cid:binFile.cidList)  {
//        for(int cid=0;cid<1;cid++)  {
        // int cid = 2;
            final String path = String.format(Define.ROOT_PATH+name+"/%02d.pes",cid );
        final int id = cid;
        //匿名类的方式实现多线程
        Runnable r = new Runnable() {
            @Override
            public void run() {
                try {
                    PESFile pesFile = new PESFile(path,id);//读取pes文件
                    //往哈希表files中添加数据（键值对）
                    files.put(id,pesFile);
                    flags.add("Succeed");
                }catch (PEError e){
                    flags.add("Failed");
                }
            }
        };
        //r.run();
        Thread tt = new Thread(r);
        tt.start();
    }
    Integer errorNum = 0;//记录读取bin文件错误的摄像头个数
    while (true){
             if(flags.size() == binFile.info.camCount){
//                 if(flags.size() == 8){
//            for(int i=0;i<binFile.info.camCount;i++){
//                if(flags.get(i) == "Failed"){
//                    errorNum++;
//                }
//            }
                 break;
             }
         }

//         for (Integer cid:binFile.cidList){
//             Log.d(String.valueOf(files.get(cid).timestampList.size()), files.get(cid).timestampList.get(files.get(cid).timestampList.size()-1).toString());
//         }
//         for (Integer cid:binFile.cidList){
//             Log.d("duration", files.get(cid).duration.toString());
//         }

//         //时间戳
//         float min = files.get(0).timestampList.get(files.get(0).timestampList.size()-1);
//         for (int i=0;i<binFile.cidList.size();i++){//遍历8个pes文件
//             if (files.get(i).duration<min){
//                 min = files.get(i).duration;//获取最小时间间隔
//             }
//         }
         //
         float min = files.get(0).duration;//获取第一个pes文件的时间间隔//175.34999
         for (int i=1;i<binFile.cidList.size();i++){//遍历剩下7个pes文件
             if (files.get(i).duration<min){
                 min = files.get(i).duration;//获取最小时间间隔并赋值给min
             }
         }
         minDuration = min ;//存储时间间隔最小的那个pes文件的时间间隔//173.257
         Log.d("minTimeStamp", String.valueOf(min));
         if(errorNum == 0)
             System.out.println("全部读取完！");
    }
     //初始化解码器
     HashMap<Integer,PESDecodeThread> initDecoders(PEDecodeBuffer decodeBuffer ) throws PEError{
        PESFile pesFile;
        HashMap<Integer,PESDecodeThread> thread = new HashMap<>();
         for (Integer cid:binFile.cidList){
            pesFile = files.get(cid);//根据镜头id获取对应的pes文件类对象
            if(pesFile == null){
                throw PEError.gen(PEErrorType.fileReadFailed,String.format("找不到pes文件，cid：%02d",cid));
            }
            //获取主码流尺寸
            Size size = new Size(binFile.camConfigs.get(cid).fullSize.width,binFile.camConfigs.get(cid).fullSize.height);
            PEFrameBuffer buffer =(PEFrameBuffer) decodeBuffer.bufferDict.get(cid);//根据cid获取帧数据缓存类对象
            try {
                PEDecoder decoder = new PEDecoder(buffer,size);//构造解码器decoder
                PESDecodeThread decodeThread = new PESDecodeThread(decoder,pesFile,cid);//解码线程
                thread.put(cid,decodeThread);//将解码线程添加到镜头-解码线程对照表
            }catch (PEError e){
                e.printStackTrace();//在命令行打印异常信息在程序中出错的位置及原因。
            }
        }
        return thread;
    }
}
