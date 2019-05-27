package panoeye.pelibrary;

import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.zip.Inflater;

/**
 * Created by Administrator on 2017/10/30.
 */

public class PERFile {
    private static final String TAG = "PERFile";
    PER_FILE_Head per_file_head = new PER_FILE_Head();
//    Float firstIFrameTimestamp = 0.0f;//第一个I帧时间戳
    int firstIFramePosition;//第一个I帧位置//2933456
    int currentFramePosition;//当前帧文件指针位置
    List<Float> iFrameTimestampList = new ArrayList<>();//I帧时间戳列表
    HashMap<Float,Integer> iFrameTimestampDict = new HashMap<>();//I帧时间戳-文件指针位置对照表
    ByteBuffer oneFrameData;//一帧数据
    Float duration = 0.0f;//第一个I帧到最后一帧之间的时间间隔，单位为秒。

    int indexCount = 0;
    int mainStreamIndexCount = 0;
    public RandomAccessFile fp;//文件指针
    PEBinFile binFile ;//bin文件
//    PERFileStatus status = PERFileStatus.closed;//pes文件状态
    PERModule PERModule[];

    //构造函数
    PERFile(String path) throws PEError {
        try {
            //打开视频文件读取到fp中
            fp = PEFile.fopen(path);
//            status = PERFileStatus.opened;
            PEFile.fseek(fp, 21, FileSeekFlag.SET);//跳过21个字节，即从第22个字节开始读取
            per_file_head.stream_payload_order = PEFile.freadInteger(fp);//读取4个字节到stream_payload_order//518163
            per_file_head.stream_payload_size = PEFile.freadInteger(fp);//读取4个字节到stream_payload_size//267885661

            PEFile.fseek(fp, Define.order_PER_serial_no, FileSeekFlag.SET);//跳过363个字节，即从第364个字节开始读取
            String serial_no = PEFile.freadString(fp,64);//读取4个字节到bin_zip_order//432
            int bin_zip_order = PEFile.freadInteger(fp);//读取4个字节到bin_zip_order//432
            int bin_zip_size = PEFile.freadInteger(fp);//读取4个字节到bin_zip_size//517731

            PEFile.fseek(fp, 371, FileSeekFlag.SET);//跳过371个字节
            byte magic_number = PEFile.freadByte(fp);
            float create_time = PEFile.freadFloat(fp);
            PEFile.fseek(fp, 12, FileSeekFlag.CUR);//跳过12个字节
            int record_start_time = PEFile.freadInteger(fp);
            PEFile.fseek(fp, 12, FileSeekFlag.CUR);//跳过12个字节
            int record_end_time = PEFile.freadInteger(fp);
            //PEFile.fseek(fp, 12, FileSeekFlag.CUR);//跳过12个字节

            int dur = record_end_time-record_start_time;//获取录像时间间隔//181
            duration = (float)dur;//181.0
            PEFile.fseek(fp, Define.order_PER_index_list_order, FileSeekFlag.SET);//跳过420个字节，即从第421个字节开始读取
            int index_list_order = PEFile.freadInteger(fp);//读取4个字节到index_list_order//268403824
            int index_list_size = PEFile.freadInteger(fp);//435
            int index_list_iter_size = PEFile.freadInteger(fp);//45


            PEFile.fseek(fp,bin_zip_order, FileSeekFlag.SET);//获取文件指针fp当前位置
            ByteBuffer zlibBinDataBuffer;
            zlibBinDataBuffer = PEFile.freadByteBuffer(fp,bin_zip_size);//读取zlibBin压缩数据数据到Buffer

            byte[] zlibBinDataIn = new byte[zlibBinDataBuffer.capacity()];
            zlibBinDataBuffer.get(zlibBinDataIn,0,zlibBinDataIn.length);

            byte[] zlibBinDataOut = decompress(zlibBinDataIn);//解压
            //新建一个File，传入文件夹目录
            File file = new File(Define.PARAM_PATH);
            //判断文件夹是否存在，如果不存在就创建，否则不创建
            if (!file.exists()) {
                file.mkdir();
            }
            File bFile = new File(Define.PARAM_PATH+serial_no+".bin");
            if (!bFile.exists()) {
                RandomAccessFile fpZlib = PEFile.fopenMode(Define.PARAM_PATH+serial_no+".bin","rw");//打开bin文件
                PEFile.fwriteArray(fpZlib,zlibBinDataOut);
                PEFile.fclose(fpZlib);//释放bin文件
            }

            //读取bin文件
            binFile = new PEBinFile(Define.PARAM_PATH+serial_no+".bin");
            PERModule = new PERModule[binFile.info.camCount];//初始化模组数组
            for (int i=0;i<binFile.info.camCount;i++){
                PERModule[i] = new PERModule();//初始化每一个模组
            }
            PEFile.fseek(fp, index_list_order, FileSeekFlag.SET);//跳过268403824个字节，即从第268403825个字节处开始读取
            buildIFrameIndex(fp);//建立I帧的时间戳-帧数据指针位置索引表

            //读取cmb文件
            String cmbPath = Define.PARAM_PATH+serial_no+".cmb";
            PECmbFile cmbFile = null;
            File cFile = new File(cmbPath);
            if (cFile.exists()) {
                cmbFile = new PECmbFile(cmbPath);
                new PEColorTune(cmbFile);
                PEColorTune.isNeedOrgImg =true;
                PEColorTune.orgImgReadyList = new int[]{0, 0, 0, 0, 0, 0, 0, 0};
                PEColorTune.isGainReady = false;
                Define.isCmbExist = true;
            } else {
                Log.e(TAG, "PERFile:找不到cmb调色文件");
                Define.isCmbExist = false;
            }
        } catch (PEError e) {
            e.gen(PEErrorType.fileReadFailed, "PERFile读取失败");
        }
    }

    //建立I帧的时间戳-帧数据指针位置索引表
    void buildIFrameIndex(RandomAccessFile fp) throws PEError {
        PER_IndexIter per_indexIter = new PER_IndexIter();
        int stream_type;
        int channel;
        float timeStamp;
        int fpPosition;
        boolean once = true;
//        for (int i = 1;  i<= 16; i++) {
        while (true) {
            per_indexIter.magic_number = PEFile.freadByte(fp);//读取1个字节并赋值给magic_number
            if (per_indexIter.magic_number == 0x42) {
                per_indexIter.pre_index_order = PEFile.freadInteger(fp);//
                per_indexIter.next_index_order = PEFile.freadInteger(fp);//
                per_indexIter.stream_packet_order = PEFile.freadInteger(fp);//
                fpPosition = per_file_head.stream_payload_order + per_indexIter.stream_packet_order;//
                PEFile.fseek(fp, 16, FileSeekFlag.CUR);//跳过16个字节，读取cPDt(即per_pe_head_.fPTS)
                per_indexIter.cPDt = PEFile.freadFloat(fp);//
                timeStamp = per_indexIter.cPDt;
                per_indexIter.is_key = PEFile.freadByte(fp);//
                per_indexIter.stream_id = PEFile.freadByte(fp);//
                PEFile.fseek(fp, 2, FileSeekFlag.CUR);//跳过2个空字节
                per_indexIter.frame_index = PEFile.freadInteger(fp);//
                per_indexIter.keyframe_index = PEFile.freadInteger(fp);//
                indexCount++;

                stream_type = per_indexIter.stream_id & 0xF0;//0xE0
                if (stream_type == 0xE0){//0xE0(即224)表示主码流
                    if(once == true) {
//                        firstIFrameTimestamp = timeStamp;//1.73
                        firstIFramePosition = fpPosition;//2933456
                        currentFramePosition = fpPosition;
                        once = false;
                    }
                    mainStreamIndexCount++;
                    channel = per_indexIter.stream_id & 0x0F;//镜头模组id//3//4
//                    Log.e("PERFile", "index:"+mainStreamIndexCount+";channel:"+channel+";timeStamp:"+timeStamp);
                    PERModule[channel].setFirstIFrameTimestamp(timeStamp);//设置每一个pesFile的头一个I帧时间戳
                    iFrameTimestampList.add(timeStamp);//构建I帧时间戳列表
                    iFrameTimestampDict.put(timeStamp,fpPosition);//构建I帧时间戳-文件指针位置对照表
                }
            }
            else {
                Log.d("PERFile", "findFirstIFrame:perFile总索引大小："+indexCount);//435
                Log.d("PERFile", "findFirstIFrame:perFile主码流索引大小："+mainStreamIndexCount);//216(即27*8)
                break;//索引读完，跳出while循环
            }
        }
    }
    //找到I帧的位置
    int findOneFrame(RandomAccessFile fp) throws PEError {
        int stream_id;
        int stream_type;
        int channel;
        int cid;
        float timeStamp;
        Integer length;
        PEFile.fseek(fp, currentFramePosition, FileSeekFlag.SET);//从2933456位置出开始读取帧数据
        while(true) {
            if (PEFile.perHeadEquals(fp)) {
                Integer pointTemp = PEFile.ftell(fp);//读取当前文件指针位置到临时变量//2933460//3021947//3035100
                PEFile.fseek(fp, 7, FileSeekFlag.CUR);//跳过7个字节，读取stream_id
                stream_id = PEFile.freadByte(fp);//读取1个字节并赋值给stream_id//227(即E3)//228
                PEFile.fseek(fp, 24, FileSeekFlag.CUR);//跳过24个字节，读取fPTS
                timeStamp = PEFile.freadFloat(fp);//读取4个字节(fPTS)timeStamp//
                PEFile.fseek(fp, 24, FileSeekFlag.CUR);//跳过24个字节，读取RAW_Data_Size
                length = PEFile.freadInteger(fp);//读取4个字节(读取RAW_Data_Size)并赋值给length//88407//10952//9816
                PEFile.fseek(fp, 12, FileSeekFlag.CUR);//跳过12个字节，即RAW_Data开始部分

                stream_type = stream_id & 0xF0;//224
                if (stream_type == 0xE0)//224(即0xE0)表示主码流
                {
                    channel = stream_id & 0x0F;//镜头模组id//6//2//5
                    if (timeStamp< PERModule[channel].getFirstIFrameTimestamp()) {
                        PEFile.fseek(fp, length, FileSeekFlag.CUR);//跳过length个字节
                        currentFramePosition = PEFile.ftell(fp);//读取当前文件指针位置到framePosition
                        return -1;
                    }
                    cid = channel;
//                    switch (binFile.info.camCount){
//                        case 5:
//                            if (channel==1)
//                                cid = 3;
//                            else if (channel==3)
//                                cid = 1;
//                            else
//                                cid = channel;
//                            break;
//                        case 8:
//                            if (channel>0 && channel<7)
//                                cid = 7-channel;
//                            else
//                                cid = channel;
//                            break;
//                        default:
//                                cid = channel;
//                            break;
//                    }

//                    Log.d("PERFile", "channel:" + channel + ";timeStamp:" + timeStamp + ";point: " + pointTemp + ";length: " + length);
                    oneFrameData = PEFile.freadByteBuffer(fp, length);//读取一帧数据

//                    RandomAccessFile fpYUV = PEFile.fopenMode(Define.ROOT_PATH +"E10010078.one","rw");//打开文件
//                    PEFile.fwriteArray(fpYUV,oneFrameData);
//                    PEFile.fclose(fpYUV);//释放文件

                    currentFramePosition = PEFile.ftell(fp);//读取当前文件指针位置到framePosition
                    return cid;
                } else {
                    PEFile.fseek(fp, length, FileSeekFlag.CUR);//读到子码流，跳过一帧
                }
            } else {
                Log.d("PERFile", "findOneFrame: 帧数据读取到达末尾");
                return -1;
            }
        }
    }

    /**
     * 解压缩
     * @param data 待解压缩的数据
     * @return byte[] 解压缩后的数据
     */
    public static byte[] decompress(byte[] data) {
        byte[] output = new byte[0];//定义byte数组用来放置解压后的数据

        Inflater decompresser = new Inflater();
        decompresser.reset();
        decompresser.setInput(data);//设置当前输入解压
        //decompresser.setInput(data, 0, data.length);
        ByteArrayOutputStream o = new ByteArrayOutputStream(data.length);
        try {
            byte[] buf = new byte[1024];
            while (!decompresser.finished()) {
                int i = decompresser.inflate(buf);
                o.write(buf, 0, i);
//                Log.d("while循环", "i: "+i);//1024
            }
            output = o.toByteArray();
        } catch (Exception e) {
            output = data;
            e.printStackTrace();
        } finally {
            try {
                o.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        decompresser.end();
        return output;
    }
}



