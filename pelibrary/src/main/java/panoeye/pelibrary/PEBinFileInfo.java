package panoeye.pelibrary;

import java.io.RandomAccessFile;

/**
 * Created by tir on 2016/12/8.
 */

public class PEBinFileInfo {
     public Integer radius;// 半径（范围）
     String binName ;//bin文件名（全景序列号）
     Integer model_type,sub_model_type;//相机类型、子类型
     public Integer camCount;//镜头模组个数
    //构造函数
     PEBinFileInfo(RandomAccessFile fp) throws PEError{
        PEFile.fseek(fp,264, FileSeekFlag.SET);//文件指针fp定位到264的位置
        radius = PEFile.freadInteger(fp);//通过文件指针fp读取4个字节 //450
        int off = 256 + 4*7 + 16 + 4 + 64;//368
        PEFile.fseek(fp,off,FileSeekFlag.SET); //文件指针fp定位到368的位置
        binName = PEFile.freadString(fp,64);//通过文件指针fp读取64个字节 //E10010175
        model_type = (int) binName.charAt(0)-65;//第一个字符-A
        sub_model_type = (int) binName.charAt(1)-48;//第二个字符-0
        int off1 = off + 64 +16*4 +16*1 + 11*4 + 10*4 + 916;//1512
        PEFile.fseek(fp,off1,FileSeekFlag.SET);//文件指针fp定位到1512的位置
        camCount = PEFile.freadInteger(fp);//通过文件指针fp读取4个字节 获取相机镜头模组个数

    }
    //构造函数
     PEBinFileInfo(String str) throws PEError{
        this(PEFile.fopen(str));//构造函数只可调用一次，且放在第一位
    }
    //默认构造函数
     PEBinFileInfo(){}


}
