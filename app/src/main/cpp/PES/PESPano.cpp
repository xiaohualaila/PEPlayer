//
// Created by tir on 2016/9/1.
//
#include "PESPano.h"
#include "../MainController.h"

static void sendSetPESTask(void ** objs) {
    PESPano * pano = (PESPano *)objs[0];
    char * filename = (char *)objs[1];
    int code = *(int *)objs[2];
    int * successCountPtr = (int *)objs[3];
    int * doneThreadCountPtr = (int *)objs[4];
    pano->setPESTask(filename, code,doneThreadCountPtr, successCountPtr);
}

void PESPano::setPESTask(char *filename, int code,int * doneThreadCountPtr, int *successCountPtr) {
    if (setPES(filename, code) == true) {
        LOGI("PANO: Set pes decoder success in %s", filename);
        pthread_mutex_lock(&threadMutex);
        (*successCountPtr) = (*successCountPtr) + 1;
        pthread_mutex_unlock(&threadMutex);
    } else {
        LOGI("PANO: Set pes decoder failed in %s.", filename);
    }
    pthread_mutex_lock(&threadMutex);
    (*doneThreadCountPtr) = (*doneThreadCountPtr) + 1;
    pthread_mutex_unlock(&threadMutex);
}

PESPano::PESPano() {
    binFile = NULL;
    for (int i = 0; i < MAX_CAMERA_COUNT; i++) {
        decoders[i] = NULL;
    }
    decoderCount = 0;
    totalDuration = 0;
    decodeFrameRate = 0;
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&threadMutex,NULL);
}

PESPano::~PESPano() {
    if (binFile != NULL) {
        delete binFile;
    }
    for (int i = 0; i < MAX_CAMERA_COUNT; i++) {
        if (decoders[i] != NULL) {
            delete decoders[i];
        }
    }
}

bool PESPano::setPano(char * filename){
    binFile = new PESBinFile();
    char * tPath = "/storage/emulated/0/Pano/";
    char path[64] = {0};
    strcat(path, tPath);
    strcat(path, filename);
    strcat(path, "/");
    char binPathFilename[128] = {0};
    strcat(binPathFilename, path);
    strcat(binPathFilename, filename);
    strcat(binPathFilename, ".bin");
    if (binFile->openFile(binPathFilename) == false) {
        LOGI("PANO: Open bin file faild in %s.", binPathFilename);
        return false;
    }
    LOGI("PANO: Open bin file success in %s.", binPathFilename);
    int camCount = binFile->camCount;
    if (camCount < MIN_CAMERA_COUNT && camCount > MAX_CAMERA_COUNT) {
        mc->setErrorMessage("不支持的视频分块数！");
        LOGI("PANO: Not avaliable camera count in %d.", camCount);
        return false;
    }
    int codeList[MAX_CAMERA_COUNT];
    //读取PES文件
    char pesPathFilename[MAX_CAMERA_COUNT][128];
    void * objs[MAX_CAMERA_COUNT][5];
    int doneThreadCount = 0;
    int successCount = 0;
    for (int i = 0; i < camCount; i++) {
        int code = binFile->index[i];
        char codeChar[3] = { 0 };
        codeChar[0] = '0';
        codeChar[1] = code + '0';
        pesPathFilename[i][0] = 0;
        strcat(pesPathFilename[i], path);
        strcat(pesPathFilename[i], codeChar);
        strcat(pesPathFilename[i], ".pes");
        objs[i][0] = this;
        objs[i][1] = pesPathFilename[i];
        codeList[i] = code;
        objs[i][2] = &codeList[i];
        objs[i][3] = &successCount;
        objs[i][4] = &doneThreadCount;
    }
    //创建线程
    for (int i = 0; i <camCount; i++) {
        pthread_create(&thread[i], NULL, (void * (*)(void *))&sendSetPESTask, (void *)&objs[i]);
    }
    //等待回归
    while (doneThreadCount < camCount) {
        usleep(100000);
    }
    if (successCount < camCount) {
        return false;
    }
    //统一时间0点，把开头的时间减掉
    float maxStartTS = -1;
    for (int i = 0; i < camCount; i++) {
        int code = binFile->index[i];
        PESFile * pesFile = decoders[code]->pesFile;
        float startTS = pesFile->tsList[0];
        if (startTS > maxStartTS) {
            maxStartTS = startTS;
        }
    }
    //扫描时间戳
    for (int i = 0; i < camCount; i++) {
        int code = binFile->index[i];
        PESFile * pesFile = decoders[code]->pesFile;
        for (int j = 0; j < pesFile->timestampCount; j++) {
            int mapValue = pesFile->timestampMap[pesFile->tsList[j]];
            pesFile->timestampMap.erase(pesFile->tsList[j]);
            pesFile->tsList[j] -= maxStartTS;
            pesFile->timestampMap[pesFile->tsList[j]] = mapValue;
        }
    }
    //找到总时间
    float minTotalTS = 1.0e+10;          //取个足够大的数
    for (int i = 0; i < camCount; i++) {
        int code = binFile->index[i];
        PESFile * pesFile = decoders[code]->pesFile;
        float totalTS = pesFile->tsList[pesFile->timestampCount - 1];
        if (totalTS < minTotalTS) {
            minTotalTS = totalTS;
        }
    }
    totalDuration = minTotalTS;

    return true;
}


bool PESPano::setPES(char * filename, int code){
    PESDecoder * decoder = new PESDecoder();
    if (decoder->setFile(filename, code, binFile->width, binFile->height) == false) {
        delete decoder;
        decoder = NULL;
        return false;
    } else {
        decoders[code] = decoder;
        decoderCount++;
        return true;
    }
}

void PESPano::run(){
    for (int i = 0; i < decoderCount; i++) {
        decoders[binFile->index[i]]->run();
    }
}

float PESPano::jumpToTimeline() {
    float minTimestamp = 1e+10;
    for (int i = 0; i < decoderCount; i++) {
        int code = binFile->index[i];
        float timestamp = decoders[code]->jumpTo(mc->timeline);
        if (timestamp < minTimestamp) {
            minTimestamp = timestamp;
        }
    }
    return minTimestamp;
}

void PESPano::awakeDecoders() {
    for (int i = 0; i < decoderCount; i++) {
        pthread_cond_signal(&decoders[binFile->index[i]]->cond);    //唤醒解码线程
    }
}

void PESPano::resume(){
    for (int i = 0; i < decoderCount; i++) {
        decoders[binFile->index[i]]->resume();
    }
}

void PESPano::pause(){
    for (int i = 0; i < decoderCount; i++) {
        decoders[binFile->index[i]]->pause();
    }
}

void PESPano::stop(){
    for (int i = 0; i < decoderCount; i++) {
        decoders[binFile->index[i]]->stop();
    }
}

bool PESPano::setStreamType(int type) {
    mc->setErrorMessage("PES视频中不包含子码流数据！");
    return false;
}

void PESPano::decodeFrameRateStep() {
    pthread_mutex_lock(&mutex);
    decodeFrameRate++;
    pthread_mutex_unlock(&mutex);
}

int PESPano::getDecodeFrameRate() {
    pthread_mutex_lock(&mutex);
    int fr = (int)((float)decodeFrameRate / (float)binFile->camCount + 0.5);
    decodeFrameRate = 0;
    pthread_mutex_unlock(&mutex);
    return fr;
}