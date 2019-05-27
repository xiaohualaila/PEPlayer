package panoeye.pelibrary;



import android.util.Log;

import java.io.RandomAccessFile;
import java.nio.ByteBuffer;

/**
 * Created by tir on 2016/12/12.
 */

 class PEVideoPacket {
     ByteBuffer oneFrameData;//一帧数据
     Float timeStamp ;//时间戳
     Integer length;//长度
    int stream_id;
     //构造函数,根据文件指针fp位置获取一帧数据包
     PEVideoPacket(RandomAccessFile fp,Integer point){
        try {
            PEFile.fseek(fp,point, FileSeekFlag.SET);//获取文件指针fp当前位置//8442218
            PEFile.fseek(fp, 60, FileSeekFlag.CUR);//跳过60个字节，读取RAW_Data_Size

//            PEFile.fseek(fp, 7, FileSeekFlag.CUR);//跳过7个字节，读取stream_id
//            stream_id = PEFile.freadByte(fp);//读取1个字节并赋值给stream_id//227(即E3)//228
//            PEFile.fseek(fp, 24, FileSeekFlag.CUR);//跳过24个字节，读取fPTS
//            timeStamp = PEFile.freadFloat(fp);//读取4个字节(fPTS)timeStamp//
//            PEFile.fseek(fp, 24, FileSeekFlag.CUR);//跳过24个字节，读取RAW_Data_Size

            length = PEFile.freadInteger(fp);//读取4个字节(读取RAW_Data_Size)并赋值给length//74553
            PEFile.fseek(fp, 12, FileSeekFlag.CUR);//跳过12个字节，即RAW_Data开始部分

//            if (point == 2933460){
//                Log.d("PEVideoPacket", "length: "+length);//88407//1010385479
//            }
//            Log.d("PEVideoPacket", "point:"+point);//8442218//9279248//2933460//9612621
//            Log.d("PEVideoPacket", "length: "+length);//74553//55604//88407//72190//..//64
            if (length>100498||length<0){
                Log.d("PEVideoPacket", "length: "+length);
            }else {
                oneFrameData = PEFile.freadByteBuffer(fp,length);//读取一帧数据
            }
        }catch (PEError e){
            e.gen(PEErrorType.fileReadFailed,"帧读取失败");
        }

    }
}
