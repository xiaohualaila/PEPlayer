package panoeye.pelibrary;



import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Created by tir on 2016/12/9.
 */
//pes文件类
 class PESFile {
    Float duration = 0.0f;//第一个I帧到最后一帧之间的时间间隔，单位为秒。
    private byte x;//帧类型，103代表I帧，97代表P帧。
    private Float timestamp;//时间戳
    List<ByteBuffer> oneFrameDataList = new ArrayList<>();//数据帧列表

    List<Float> timestampList = new ArrayList<>();//时间戳列表
    HashMap<Float,Integer> timestampDict = new HashMap<>();//时间戳-文件指针位置对照表
    List<Float> IFrame = new ArrayList<>();//I帧列表
    Float firstPacketTimestampOffset = 0.0f;//第一个I帧时间戳
//    PERFileStatus status = PERFileStatus.closed;//pes文件状态
    RandomAccessFile fp;//文件指针
     int id;
    //构造函数
    PESFile(String path,int id) throws PEError{
        try {
            //打开视频文件读取到fp中
            fp = PEFile.fopen(path);
            this.id = id;
//            status = PERFileStatus.opened;
            PEFile.fseek(fp, Define.SIZE_PES_HEAD, FileSeekFlag.SET);//跳过2000个字节，即从第2001个字节开始读取
            buildData(fp);//取出视频数据
        }catch (PEError e){
            e.gen(PEErrorType.fileReadFailed,"PESFile读取失败");
        }
    }
     //根据帧编号获取视频包的该帧数据
     PESVideoPacket getPESVideoPacket(Integer frameNum){//frameNum==0
        timestamp = timestampList.get(frameNum);//5.451
        Integer point = timestampDict.get(timestamp);//872824
        return new PESVideoPacket(fp,point);//返回一帧数据
    }
//    // 帧编号获取视频包的该帧数据
//    ByteBuffer getPEVideoPacket(Integer frameNum){
//        return oneFrameDataList.get(frameNum);//返回一帧数据
//    }

     //建立时间戳-帧数据指针位置对照表
     void buildData(RandomAccessFile fp)throws PEError {
        Integer fLength = PEFile.flength(fp);
        findIFrame(fp);//找到第一个I帧的位置
        findFrame(fp);//找到余下所有帧的位置
    }
    //找到第一个I帧的位置
     void findIFrame(RandomAccessFile fp)throws PEError{
        Integer length;
         int icount = 0,ecount=0;
        while (true) {
            if (PEFile.headEquals(fp)) {//若Head正确
                icount++;
                Integer pointtemp = PEFile.ftell(fp);//读取当前文件指针位置到临时变量//2005//10109
                Float timeStamp = PEFile.freadFloat(fp);//读取4个字节到临时变量timeStamp
                length = PEFile.freadInteger(fp);//读取4个字节到length//8091
                PEFile.fseek(fp,4, FileSeekFlag.CUR);//跳4个字节，判断I帧
                x = PEFile.freadByte(fp);//读取1个字节到x//0x61(即97)//...//0x67(即103)
                if( (x & 0x1f)==0x07){//当x=103找到I帧,第一个
                    timestampList.add(timeStamp);//存储时间戳
                    timestampDict.put(timeStamp,pointtemp);//时间戳和指针位置
                    IFrame.add(timeStamp);
                    firstPacketTimestampOffset = timeStamp;
                    PEFile.fseek(fp,-5,FileSeekFlag.CUR);//返回5个字节，回到读取完length的位置
                    PEFile.fseek(fp,length,FileSeekFlag.CUR);//跳过一帧
//                    ByteBuffer oneFrameData = PEFile.freadByteBuffer(fp,length);//读取一帧数据到临时变量
//                    oneFrameDataList.add(oneFrameData);//添加到帧数据列表
                    break;//跳出循环
                }
                else{//没找到
                    PEFile.fseek(fp,-5,FileSeekFlag.CUR);//返回5个字节，回到读取完length的位置
                    PEFile.fseek(fp,length,FileSeekFlag.CUR);//跳过一帧,继续找
                }
            }
            else
                ecount++;
        }
    }
    //找到余下所有帧的位置
    void findFrame(RandomAccessFile fp)throws PEError{
        Integer length;
        int icount2 = 0,ecount2=0;
        while (true){
            if(PEFile.headEquals(fp)){
                icount2++;
                Integer pointtemp = PEFile.ftell(fp);//读取当前文件指针位置到临时变量
                Float timeStamp = PEFile.freadFloat(fp);//保存时间戳到临时变量
                length = PEFile.freadInteger(fp);//读取length
                //如果文件读到了结尾执行if语句体。其中flength()获取文件长度
                if(PEFile.flength(fp)<=(PEFile.ftell(fp)+length)){
                    int ffLength = PEFile.flength(fp);
                    int tell = (PEFile.ftell(fp));
                    //获取第一个I帧到最后一帧之间的时间间隔
                    duration = timestampList.get(timestampList.size()-1) - firstPacketTimestampOffset;
                    return;
                }
                timestampList.add(timeStamp);
                timestampDict.put(timeStamp,pointtemp);

                PEFile.fseek(fp,4, FileSeekFlag.CUR);//跳4个字节，判断I帧
                x = PEFile.freadByte(fp);
                if ((x & 0x1f)==0x07){
                    IFrame.add(timeStamp);
                }
                PEFile.fseek(fp,-5,FileSeekFlag.CUR);//返回5个字节，回到读取完length的位置
                PEFile.fseek(fp,length,FileSeekFlag.CUR);//跳过一帧,继续读取

//                ByteBuffer oneFrameData = PEFile.freadByteBuffer(fp,length);//读取一帧数据到临时变量
////                oneFrameDataList.add(oneFrameData);//添加到帧数据列表

//                RandomAccessFile fpH264 = PEFile.fopenMode(Define.ROOT_PATH+"/h264/"+id+".h264","rw");//打开文件
//                // 文件长度，字节数
//                long fileLength = 0;
//                try {
//                    fileLength = fpH264.length();
//                    // 将写文件指针移到文件尾。
//                    fpH264.seek(fileLength);
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//                PEFile.fwriteArray(fpH264,oneFrameData);
//                PEFile.fclose(fpH264);//释放文件
            }else
                ecount2++;
        }
    }

}
