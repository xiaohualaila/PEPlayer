package panoeye.pelibrary;



import java.io.RandomAccessFile;
import java.nio.ByteBuffer;

/**
 * Created by tir on 2016/12/12.
 */

 class PESVideoPacket {
     ByteBuffer oneFrameData;//一帧数据
     ByteBuffer head;//头
     Float timeStamp ;//时间戳
     Integer length;//长度
     FrameType frameType ;//帧类型
     //构造函数
     PESVideoPacket(RandomAccessFile fp,Integer point){
        try {
            PEFile.fseek(fp,point, FileSeekFlag.SET);//获取文件指针fp当前位置
            timeStamp = PEFile.freadFloat(fp);//通过文件指针fp读取4个字节
            length = PEFile.freadInteger(fp); //通过文件指针fp读取4个字节

            head = PEFile.freadByteBuffer(fp,5);//通过文件指针fp读取5个字节
            PEFile.fseek(fp,-1,FileSeekFlag.CUR);//文件指针fp左移1个字节

            byte x = PEFile.freadByte(fp);//通过文件指针fp读取1个字节
            if((x&0x1f)==0x07 ){
                frameType = FrameType.SPS;//序列参数集
            }
            if((x&0x1f)==0x08 ){
                frameType = FrameType.PPS;//图像参数集
            }
            if((x&0x1f)==0x05 ){
                frameType = FrameType.IDR;//即时解码器刷新
            }

            PEFile.fseek(fp,-5,FileSeekFlag.CUR);//返回5个字节，回到读取完length的位置

            //PEFile.fseek(fp,8,FileSeekFlag.CUR);
            oneFrameData = PEFile.freadByteBuffer(fp,length);//读取一帧数据

//            RandomAccessFile fpH264 = PEFile.fopenMode(Define.ROOT_PATH +"E10010078.h264","rw");//打开文件
//            PEFile.fwriteArray(fpH264,oneFrameData);
//            PEFile.fclose(fpH264);//释放文件

        }catch (PEError e){
            e.gen(PEErrorType.fileReadFailed,"帧读取失败");
        }

    }

}
