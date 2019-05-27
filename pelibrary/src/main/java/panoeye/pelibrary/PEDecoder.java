package panoeye.pelibrary;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;

import static panoeye.pelibrary.PEColorTune.getOrgImg;

/**
 * Created by tir on 2016/12/14.
 * 编解码器类
 * https://www.cnblogs.com/roger-yu/p/5635494.html
 */

public class PEDecoder {
    private static final String TAG = "PEDecoder";
    MediaCodec mediaCodec;//Codec 编码解码器
    MediaCodec.BufferInfo info ;//编码解码器的缓冲区信息
    PEFrameBuffer frameBuffer;//解码前存放一帧待解码的数据+配置，解码后存放解码出来的数据+配置
    ByteBuffer buffer;//临时存放一帧解码出来的数据
    Size size;
    boolean isHuawei=false;
    int tt=0;
    RandomAccessFile fpYUV;
    //构造函数：初始化解码器
    public PEDecoder(PEFrameBuffer buffer, Size size)throws PEError{
        try {
            frameBuffer = buffer;
            this.size = size;
            info = new MediaCodec.BufferInfo();
            mediaCodec= MediaCodec.createDecoderByType("video/avc");//avc 自动音量控制

            String tt= Build.BRAND;
            if (tt.equals("HUAWEI"))
            {
                //isHuawei = true;
            }
            configure();
        }catch (IOException e){
            PEError.gen(PEErrorType.fileReadFailed,"解码器初始化失败!");
        }
    }
    void configure(){
        MediaFormat format = new MediaFormat();
        format.setString(MediaFormat.KEY_MIME,"video/avc");//媒体内容类型
        format.setInteger(MediaFormat.KEY_HEIGHT,size.height);
        format.setInteger(MediaFormat.KEY_WIDTH,size.width);

        format.setInteger(MediaFormat.KEY_COLOR_FORMAT,MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar);//指定待解码的YUV格式
//            format.setInteger(MediaFormat.KEY_COLOR_FORMAT,MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedPlanar);//指定待解码的YUV格式
//            format.setInteger(MediaFormat.KEY_COLOR_FORMAT,MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar);//指定待解码的YUV格式
//            format.setInteger(MediaFormat.KEY_COLOR_FORMAT,MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible);//指定待解码的YUV（默认）格式
        mediaCodec.configure(format,null,null,0);//配置编解码器
        mediaCodec.start();//启动编解码器
    }

//    public void configure(int width, int height, ByteBuffer sps, ByteBuffer pps) {
//        // width & height 需要从H264流开头的sps中解析出
//        MediaFormat mediaFormat = MediaFormat.createVideoFormat("video/avc", width, height);
//        // 这里直接把sps和pps传进去就可以了
//        mediaFormat.setByteBuffer("csd-0", sps);
//        mediaFormat.setByteBuffer("cud-1", pps);
//        String mime = mediaFormat.getString(MediaFormat.KEY_MIME);
//        try {
//            mediaCodec = MediaCodec.createDecoderByType(mime);
//        } catch (IOException e) {
//
//        }
//        mediaCodec.configure(mediaFormat, surface, null, 0);
//        mediaCodec.start();
//        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
//    }

    //解码一帧数据
    public void decode(ByteBuffer oneFrameData) throws PEError{
        ByteBuffer inputBuffer,outputBuffer;
        try {
            Integer inputBufferIndex = mediaCodec.dequeueInputBuffer(100000);// 出列输入缓冲区索引，参数（超时时间）单位为微秒（100000μs即0.1s）
            if(inputBufferIndex>=0){
                inputBuffer = mediaCodec.getInputBuffer(inputBufferIndex);// 根据索引获得输入缓冲区

    //            Log.d("PEDecoder", "decode:oneFrameData: "+oneFrameData.remaining());
    //            Log.d("PEDecoder", "decode:inputBuffer:"+inputBuffer.remaining());//7077888
    //            if (oneFrameData.remaining()<inputBuffer.remaining()){
    //                inputBuffer.put(oneFrameData);//inputBuffer填入一帧待解码的数据
    //                mediaCodec.queueInputBuffer(inputbufferIndex,0,oneFrameData.capacity(),0,MediaCodec.BUFFER_FLAG_KEY_FRAME);// 入列：在inputBufferIndex序号的位置填入一帧数据
    //            }

                inputBuffer.put(oneFrameData);// 将数据放进输入缓冲区
                mediaCodec.queueInputBuffer(inputBufferIndex,0,oneFrameData.capacity(),0,MediaCodec.BUFFER_FLAG_KEY_FRAME);// 入列：在inputBufferIndex序号的位置填入一帧数据
            }

            Integer outputBufferIndex = mediaCodec.dequeueOutputBuffer(info,100000);// 出列输出缓冲区索引，第2个参数（超时时间）单位为微秒（100000μs即0.1s）
            if(outputBufferIndex>=0){
                outputBuffer = mediaCodec.getOutputBuffer(outputBufferIndex);// 根据索引获得输出缓冲区
                if (buffer == null){
                    buffer = ByteBuffer.allocateDirect(outputBuffer.limit());//根据outputBuffer的大小分配buffer大小
                    frameBuffer.set(buffer);//设置frameBuffer的成员_imgBuffer为ByteBuffer类对象buffer
                }
                frameBuffer.lock();//PEFrameBuffer类对象frameBuffer上锁
                buffer.clear();//填入数据前 先清空缓存
                buffer.put(outputBuffer);//buffer填入一帧解码后的数据outputBuffer

                frameBuffer.isNeedReload = true;//解码好帧数据后加上标志，为后面OpenGL绘制做准备(需要判断是否有解码好的数据)
                Define.videoFrameRate++;
                Log.d(TAG, "outputBuffer-->"+outputBuffer.position());
                buffer.clear();//填入数据后 再清空缓存
                frameBuffer.unlock();//PEFrameBuffer类对象frameBuffer解锁
                mediaCodec.releaseOutputBuffer(outputBufferIndex,true);//释放输出缓存
            }
        }catch (Error e){
            PEError.gen(PEErrorType.unknow,"解码失败!");
            e.printStackTrace();
        }
    }

    public static byte[] NV12ToYuv420P(byte[] nv12,int width,int height) {
        byte[] yuv420p = new byte[nv12.length];
        int ySize = width * height;
        int i, j;
//y
        for (i =0; i < ySize; i++) {
            yuv420p[i] = nv12[i];
        }
//u
        i =0;
        for (j =0; j < ySize /2; j +=2) {
            yuv420p[ySize + i] = nv12[ySize + j];
            i++;
        }
//v
        i =0;
        for (j =1; j < ySize /2; j+=2) {
            yuv420p[ySize *5 /4 + i] = nv12[ySize + j];
            i++;
        }
        return yuv420p;
    }
    //解码一帧数据
    void decode(ByteBuffer h264Data,int cid,int index) throws PEError{
        ByteBuffer inputBuffer,outputBuffer;

        Integer inputBufferIndex = mediaCodec.dequeueInputBuffer(1000);// 出列输入缓冲区索引，参数（超时时间）单位为微秒（100000μs即0.1s）
        if(inputBufferIndex>=0){
            inputBuffer = mediaCodec.getInputBuffer(inputBufferIndex);// 根据索引获得输入缓冲区
            inputBuffer.put(h264Data);// 将数据放进输入缓冲区
            mediaCodec.queueInputBuffer(inputBufferIndex,0,h264Data.capacity(),0,MediaCodec.BUFFER_FLAG_KEY_FRAME);// 入列：在inputBufferIndex序号的位置填入一帧数据
//            switch (h264Data[4] & 0x1f) {
//                case KEY_FRAME:
//                    mediaCodec.queueInputBuffer(inputBufferIndex, 0, h264Data.capacity(), 0L, MediaCodec.BUFFER_FLAG_KEY_FRAME);
//                    break;
//                case SPS_FRAME:
//                case PPS_FRAME:
//                    mediaCodec.queueInputBuffer(inputBufferIndex, 0, h264Data.capacity(), 0L, MediaCodec.BUFFER_FLAG_CODEC_CONFIG);
//                    break;
//                default:
//                    mediaCodec.queueInputBuffer(inputBufferIndex, 0, h264Data.capacity(), 0L, 0);
//                    break;
//            }
        }

        Integer outputBufferIndex = mediaCodec.dequeueOutputBuffer(info,1000);// 出列输出缓冲区索引，第2个参数（超时时间）单位为微秒（100000μs即0.1s）
        if(outputBufferIndex>=0){
            outputBuffer = mediaCodec.getOutputBuffer(outputBufferIndex);// 根据索引获得输出缓冲区
            if (buffer == null){
                buffer = ByteBuffer.allocateDirect(outputBuffer.limit());//根据outputBuffer的大小分配buffer大小
                frameBuffer.set(buffer);//设置frameBuffer的成员_imgBuffer为ByteBuffer类对象buffer
            }
            frameBuffer.lock();//PEFrameBuffer类对象frameBuffer上锁
            buffer.clear();//填入数据前 先清空缓存
            if(isHuawei)
            {
                byte[] b = new byte[outputBuffer.remaining()];
                outputBuffer.get(b,0,b.length);
                byte[] a = NV12ToYuv420P(b,size.width,size.height);
                ByteBuffer tt = ByteBuffer.wrap(a);
                buffer.put(tt);
            }
            else
            {
                buffer.put(outputBuffer);//buffer填入一帧解码后的数据outputBuffer

            }


            if(cid==0)
            {
//                RandomAccessFile fpYUV = PEFile.fopenMode(Define.ROOT_PATH+"/yuv/"+tt+".yuv","rw");//打开文件
//                tt++;
//                PEFile.fwriteArray(fpYUV,buffer);
//                PEFile.fclose(fpYUV);//释放文件
            }

            frameBuffer.isNeedReload = true;//解码好帧数据后加上标志，为后面OpenGL绘制做准备(需要判断是否有解码好的数据)
            buffer.clear();//填入数据后 再清空缓存
            frameBuffer.unlock();//PEFrameBuffer类对象frameBuffer解锁
            mediaCodec.releaseOutputBuffer(outputBufferIndex,true);//释放输出缓存
            Define.videoFrameRate++;
        }
    }

    //解码一帧数据
    public void decode(ByteBuffer oneFrameData,int cid) throws PEError{
//        if(cid==6)
//        {
//            //RandomAccessFile fpYUV = PEFile.fopenMode(Define.ROOT_PATH+"/yuv/"+tt+".yuv","rw");//打开文件
//            if(fpYUV==null)
//            {
//                fpYUV = PEFile.fopenMode(Define.ROOT_PATH+"/yuv/"+tt+".264","rw");//打开文件
//            }
//            //tt++;
//            PEFile.fwriteArray(fpYUV,buffer);
//            PEFile.fclose(fpYUV);//释放文件
//        }


        ByteBuffer inputBuffer,outputBuffer;
        try {
            Integer inputBufferIndex = mediaCodec.dequeueInputBuffer(0);// 出列输入缓冲区索引，参数（超时时间）单位为微秒（100000μs即0.1s）
            if(inputBufferIndex>=0){
                inputBuffer = mediaCodec.getInputBuffer(inputBufferIndex);// 根据索引获得输入缓冲区

                //            Log.d("PEDecoder", "decode:oneFrameData: "+oneFrameData.remaining());
                //            Log.d("PEDecoder", "decode:inputBuffer:"+inputBuffer.remaining());//7077888
                //            if (oneFrameData.remaining()<inputBuffer.remaining()){
                //                inputBuffer.put(oneFrameData);//inputBuffer填入一帧待解码的数据
                //                mediaCodec.queueInputBuffer(inputbufferIndex,0,oneFrameData.capacity(),0,MediaCodec.BUFFER_FLAG_KEY_FRAME);// 入列：在inputBufferIndex序号的位置填入一帧数据
                //            }

                inputBuffer.put(oneFrameData);// 将数据放进输入缓冲区
                mediaCodec.queueInputBuffer(inputBufferIndex,0,oneFrameData.capacity(),0,MediaCodec.BUFFER_FLAG_KEY_FRAME);// 入列：在inputBufferIndex序号的位置填入一帧数据
            }else {
                Log.d(TAG, "decode: cid,inputBufferIndex:"+cid+","+inputBufferIndex);
            }

            Integer outputBufferIndex = mediaCodec.dequeueOutputBuffer(info,100000);// 出列输出缓冲区索引，第2个参数（超时时间）单位为微秒（100000μs即0.1s）
            if(outputBufferIndex>=0){
                outputBuffer = mediaCodec.getOutputBuffer(outputBufferIndex);// 根据索引获得输出缓冲区
                if (buffer == null){
                    buffer = ByteBuffer.allocateDirect(outputBuffer.limit());//根据outputBuffer的大小分配buffer大小
                    frameBuffer.set(buffer);//设置frameBuffer的成员_imgBuffer为ByteBuffer类对象buffer
                }

                frameBuffer.lock();//PEFrameBuffer类对象frameBuffer上锁
                buffer.clear();//填入数据前 先清空缓存
                if(isHuawei)
                {
                    byte[] b = new byte[outputBuffer.remaining()];
                    outputBuffer.get(b,0,b.length);
                    byte[] a = NV12ToYuv420P(b,size.width,size.height);
                    ByteBuffer tt = ByteBuffer.wrap(a);
                    buffer.put(tt);
                }
                else
                {
                    buffer.put(outputBuffer);//buffer填入一帧解码后的数据outputBuffer
                }
                //writePgmFormat (outputBuffer,cid);
                if (Define.isCmbExist){
                    if (PEColorTune.isNeedOrgImg){
                        getOrgImg(cid,outputBuffer);
                    }
                }

                frameBuffer.isNeedReload = true;//解码好帧数据后加上标志，为后面OpenGL绘制做准备(需要判断是否有解码好的数据)
                Define.videoFrameRate++;
//                Log.d(TAG, "decode: videoFrameRate-->"+Define.videoFrameRate);
                buffer.clear();//填入数据后 再清空缓存
                frameBuffer.unlock();//PEFrameBuffer类对象frameBuffer解锁
                mediaCodec.releaseOutputBuffer(outputBufferIndex,true);//释放输出缓存
            }else {
                Log.d(TAG, "decode: cid,outputBufferIndex:"+cid+","+outputBufferIndex);
            }
        }catch (IllegalStateException e){
            Log.e(TAG, "decode: e.printStackTrace");
            e.printStackTrace();
            Log.d(TAG, "decode: restart");
        }
        catch (Error e){
            PEError.gen(PEErrorType.unknow,"解码失败!");
            e.printStackTrace();
        }
    }

    public void stop() {
//        mediaCodec.stop();//停用编解码器
        mediaCodec.release();//释放编解码器资源
    }

    static int cnt = 0;
    int[] list = {0,0,0,0,0,0,0,0};
    public void writePgmFormat (ByteBuffer outputBuffer,int cid) throws PEError {
        Log.e(TAG, "decode: "+outputBuffer.limit());
        if (list[cid] == 0){
            list[cid] = cid+1;
            cnt++;
            Log.d(TAG, "decode: cnt:"+ cnt);
            ByteBuffer YBuf = ByteBuffer.allocateDirect(1228800);
            outputBuffer.position(0).limit(1228800);
            YBuf.put(outputBuffer);

            ByteBuffer UBuf = ByteBuffer.allocateDirect(307200);
            outputBuffer.position(1228800).limit(1536000);
            UBuf.put(outputBuffer);
            ByteBuffer VBuf = ByteBuffer.allocateDirect(307200);
            outputBuffer.position(1536000).limit(1843200);
            VBuf.put(outputBuffer);

//                    outputBuffer.position(0).limit(1228800);
//                    ByteBuffer YBuf = outputBuffer.slice();
            Log.d(TAG, "decode: "+YBuf.capacity());
            RandomAccessFile fp = PEFile.fopenMode(Define.ROOT_PATH+"/pgm/"+cid+"-"+0+".pgm","rw");//打开文件
            String version = "P5\n1280 960\n255\n";
            ByteBuffer byteBuffer = ByteBuffer.wrap(version.getBytes());
            PEFile.fwriteArray(fp,byteBuffer);
            PEFile.fwriteArray(fp,YBuf);
            PEFile.fwriteArray(fp,UBuf);
            PEFile.fwriteArray(fp,VBuf);
            PEFile.fclose(fp);//释放文件
        }
    }
}