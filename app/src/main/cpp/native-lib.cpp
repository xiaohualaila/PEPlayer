#include <jni.h>
#include <string>
#include <android/log.h>

extern "C"{
//编码
#include "libavcodec/avcodec.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"

//#include "xrapi/XrApi.h"
};

#define FFLOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"ffmpeg",FORMAT,##__VA_ARGS__);
#define FFLOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"ffmpeg",FORMAT,##__VA_ARGS__);

typedef struct ffmpeg{
    AVPacket *packet;           //包
    AVFormatContext *pFormatCtx;//格式上下文
    AVCodecContext *pCodecCtx;  //编解码上下文
    int width;                  //帧宽度
    int height ;                //帧高度
    int frame_count;            //帧数量
//    bool isUsed;                //使用标志
    const char *input;         //输入
    int v_stream_idx;           //流的类型
}ffmpegClient,*LpFfmpeg;
//int cameraCount = 0;
LpFfmpeg FFC[8];

extern "C"
JNIEXPORT jstring JNICALL
Java_com_panoeye_peplayer_PlayOnline_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
//    char info[10000] = { 0 };
//    avcodec_register_all();
//    sprintf(info, "%s\n", avcodec_configuration());
//    return env->NewStringUTF(info);
}


//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_panoeye_peplayer_OnlinePlayActivity_XrInit(
//        JNIEnv *env, jobject thiz) {
//    FFLOGI("%s","xrapiSensorInit");
//    xrapiSensorInit();
//
//    return 1;
//}

void QuaternionToEulerAngles(double qw, double qx, double qy, double qz
        , double&roll,double&yaw,double&pitch)
{
    roll = atan2f(2.f * (qw*qz + qx*qy), 1-2*(qz*qz+qx*qx)); //Z
    yaw =  asinf(2.f * (qw*qx - qy*qz)); //Y
    pitch =atan2f(2.f * (qw*qy + qz*qx), 1-2*(qy*qy+qx*qx));//X

}

//
//extern "C"
//JNIEXPORT jdoubleArray JNICALL
//Java_com_panoeye_peplayer_OnlinePlayActivity_GetData(
//        JNIEnv *env, jobject thiz) {
//
//    xrTracking tt = xrapiGetPredictedTrackingForServer(nullptr,-1);
//
//    double roll, yaw, pitch;
//    QuaternionToEulerAngles(tt.HeadPose.Pose.Orientation.w,tt.HeadPose.Pose.Orientation.x,
//                            tt.HeadPose.Pose.Orientation.y,tt.HeadPose.Pose.Orientation.z,
//                            roll, yaw, pitch);
//    jdoubleArray data= (*env).NewDoubleArray(10);
//    jdouble tmp[3];
//    tmp[0] = yaw;
//    tmp[1] = pitch;
//    tmp[2] = roll;
//    (*env).SetDoubleArrayRegion(data,0,3,tmp);
//
//    return data;
//}

extern "C"
JNIEXPORT jint JNICALL
Java_com_panoeye_peplayer_ConnectPage_Init(
        JNIEnv *env,
        jobject, /* this */
        jint cCount) {
    //1.注册所有组件
    av_register_all();
    avformat_network_init();

    for (int i = 0; i < cCount; i++) {
//        cameraCount = cCount;

        FFC[i] = new ffmpegClient;
        FFC[i]->packet = (AVPacket*)av_malloc(sizeof(AVPacket));
        FFC[i]->pFormatCtx = avformat_alloc_context();
        FFC[i]->frame_count =0;
//        FFC[i]->isUsed = false;
        FFC[i]->input = NULL;
        FFC[i]->v_stream_idx = -1;
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_panoeye_peplayer_peonline_ConnectManager_RTSPConnect(
        JNIEnv *env, jclass type, jstring input_, jint handle) {
    //获取输入输出文件名
    FFC[handle]->input = env->GetStringUTFChars(input_, 0);
    FFLOGE("%s","正在连接");

    //2.打开输入视频文件
    if (avformat_open_input(&FFC[handle]->pFormatCtx, FFC[handle]->input, NULL, NULL) != 0)
    {
        FFLOGE("%s","无法打开输入视频文件");
        return -1;
    }FFLOGI("%s","成功打开输入视频文件");

    //3.获取视频文件信息
    int ret  = avformat_find_stream_info(FFC[handle]->pFormatCtx,NULL);
    FFLOGE("avformat_find_stream_info()返回值为：%d",ret);
    if (ret < 0)
    {
        FFLOGE("%s","无法获取视频文件信息");
        return -2;
    }FFLOGI("%s","成功获取视频文件信息");

    //获取视频流的索引位置
    //遍历所有类型的流（音频流、视频流、字幕流），找到视频流

    int i = 0;
    //number of streams
    for (; i < FFC[handle]->pFormatCtx->nb_streams; i++)
    {
        //流的类型
        if (FFC[handle]->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            FFC[handle]->v_stream_idx = i;
            break;
        }
    }

    if (FFC[handle]->v_stream_idx == -1)
    {
        FFLOGE("%s","找不到视频流\n");
        return -3;
    }FFLOGI("%s","成功找到视频流");

    //只有知道视频的编码方式，才能够根据编码方式去找到解码器
    //获取视频流中的编解码上下文
    FFC[handle]->pCodecCtx = FFC[handle]->pFormatCtx->streams[FFC[handle]->v_stream_idx]->codec;
//    //4.根据编解码上下文中的编码id查找对应的解码
//    AVCodec *pCodec = avcodec_find_decoder(FFC[handle]->pCodecCtx->codec_id);
//    if (pCodec == NULL)
//    {
//        FFLOGE("%s","找不到解码器\n");
//        return -1;
//    }FFLOGI("%s","成功找到解码器");
//
//    //5.打开解码器
//    if (avcodec_open2(FFC[handle]->pCodecCtx,pCodec,NULL)<0)
//    {
//        FFLOGE("%s","解码器无法打开\n");
//        return -1;
//    }FFLOGI("%s","成功打开解码器");

    //输出视频信息
    FFLOGI("handle:%d",handle);
    FFLOGI("视频的文件格式：%s",FFC[handle]->pFormatCtx->iformat->name);
    FFLOGI("视频时长：%l", (FFC[handle]->pFormatCtx->duration)/1000000);
    FFLOGI("视频的宽高：%d,%d",FFC[handle]->pCodecCtx->width,FFC[handle]->pCodecCtx->height);
//    FFLOGI("解码器的名称：%s",pCodec->name);

    FFC[handle]->width = FFC[handle]->pCodecCtx->width;
    FFC[handle]->height = FFC[handle]->pCodecCtx->height;
    FFLOGI("连接成功：第%d路相机\n",handle);
    env->ReleaseStringUTFChars(input_,  FFC[handle]->input);
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_panoeye_peplayer_peonline_ConnectManager_getWidth(
        JNIEnv *env,
        jobject /* this */,jint handle) {
    return FFC[handle]->width;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_panoeye_peplayer_peonline_ConnectManager_getHeight(
        JNIEnv *env,
        jobject /* this */,jint handle) {
    return FFC[handle]->height;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_panoeye_peplayer_peonline_ConnectManager_getPacket(
        JNIEnv *env,
        jobject /* this */,jint handle) {

    //6.一帧一帧的读取压缩数据
    if (av_read_frame(FFC[handle]->pFormatCtx,FFC[handle]-> packet) >= 0)
    {
        if (FFC[handle]-> packet->stream_index == FFC[handle]->v_stream_idx){
            FFC[handle]->frame_count++;
            FFLOGI("获取模组：%d的第%d帧,size:%d\n",handle,FFC[handle]->frame_count, FFC[handle]->packet->size);
            //FFLOGI("获取AVPacket结构体的成员data的pointer（地址）:%p\n",FFC[handle]->packet->data);
            return env->NewDirectByteBuffer(FFC[handle]->packet->data,FFC[handle]->packet->size);
        }else{
            FFLOGE("%d:不是视频数据\n",handle);
        }
    }
    return NULL;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_panoeye_peplayer_peonline_ConnectManager_clearPacket(
        JNIEnv *env,
        jobject /* this */,jint handle) {
    //释放资源
    av_free_packet(FFC[handle]->packet);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_panoeye_peplayer_peonline_ConnectManager_clear(
        JNIEnv *env,
        jobject /* this */,jint handle) {
    //释放资源
    avformat_close_input(&FFC[handle]->pFormatCtx);
    avcodec_close(FFC[handle]->pCodecCtx);
    av_free_packet(FFC[handle]->packet);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_panoeye_peplayer_peonline_ConnectManager_closeRTSP(
        JNIEnv *env,
        jobject /* this */,jint handle) {
    //释放资源
    avformat_close_input(&FFC[handle]->pFormatCtx);
    avcodec_close(FFC[handle]->pCodecCtx);
}





