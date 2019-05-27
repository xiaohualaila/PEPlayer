//
// Created by tir on 2016/9/29.
//
#include "PERPano.h"
#include "../MainController.h"

static void runPerAwakeRunloop(void *arg) {
    mc->perPano->awakeRunloop();
}

PERPano::PERPano() {
    stoped = false;
    file = NULL;
    decoderCount = 0;
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&condMutex,NULL);
    for (int i = 0; i < MAX_CAMERA_COUNT; i++) {
        decoders[i] = NULL;
    }
}
PERPano::~PERPano() {
    for (int i = 0; i < MAX_CAMERA_COUNT; i++) {
        if (decoders[i] != NULL) {
            delete decoders[i];
        }
    }
    if (file != NULL) {
        delete file;
    }
}
bool PERPano::setPano(char * filename) {
    file = new PERFile();
    char * path = "/storage/emulated/0/Pano/";
    char pathname[512] = {0};
    strcat(pathname, path);
    strcat(pathname, filename);
    if (file->openFile(pathname) == false) {
        LOGI("PANO: Open PER file faild in %s.", pathname);
        return false;
    }
    LOGI("PANO: Open PER file success in %s.", pathname);
    int camCount = file->binFile->camCount;
    if (camCount < MIN_CAMERA_COUNT && camCount > MAX_CAMERA_COUNT) {
        mc->setErrorMessage("不支持的视频分块数！");
        LOGI("PANO: Not avaliable camera count in %d.", camCount);
        return false;
    }
    int width = file->binFile->width;
    int height = file->binFile->height;
    for (int i = 0; i < file->binFile->camCount; i++) {
        int code = file->binFile->index[i];
        PERDecoder * decoder = new PERDecoder();
        if (decoder->setDecoder(code, width, height) == false) {
            return false;
        }
        decoders[code] = decoder;
        decoderCount++;
    }
    totalDuration = file->recordHead.endTime.time - file->recordHead.startTime.time;
    return true;
}
void PERPano::run() {
    jumpToTimeline();
    for (int i = 0; i < decoderCount; i++) {

        decoders[i]->run();
    }
    pthread_create(&thread, NULL, (void * (*)(void *))&runPerAwakeRunloop, NULL);
}
float PERPano::jumpToTimeline() {
    float tl = mc->timeline;
    float minTS = 1024;
    LONG minOffset;
    long dataOffset;
    for (int i = 0; i < decoderCount; i++) {
        int code = file->binFile->index[i];
        int index = -1;
        float ts = -1024;
        for (int j = 1; j < file->indexIndex[code]; j++) {
            if (file->indexList[code][j].cPDt >= tl) {
                index = j - 1;
                ts = file->indexList[code][index].cPDt;
                dataOffset = file->indexList[code][index].streamPackageOffset;
                break;
            }
        }
        if (index == -1) {
            index = file->indexIndex[code] - 1;
            ts = file->indexList[code][index].cPDt;
            dataOffset = file->indexList[code][index].streamPackageOffset;
        }
        decoders[code]->lastTS = ts;
        if (ts < minTS) {
            minTS = ts;
            minOffset = dataOffset;
        }
    }
    LONG playDataOffset = file->head.streamOffset + minOffset;
    file->seekTo(playDataOffset);
    for (int i = 0; i < decoderCount; i++) {
        decoders[i]->isNeedJump = true;
    }
    return minTS;
}
void PERPano::awakeDecoders() {
    pthread_cond_signal(&cond);
}

void PERPano::awakeRunloop() {
    while (true) {
        pthread_cond_wait(&cond, &condMutex);                   //自身等待唤醒
        if (stoped) {
            LOGI("PERPano:awakeRunloop stoped");
            break;
        }
        for (int i = 0; i < decoderCount; i++) {
            int code;
            if (file->setVideoData(&code) == true) {
                LOGI("PERPano:awakeRunloop唤醒解码线程:%d ", i);
                pthread_cond_signal(&decoders[code]->cond);    //唤醒解码线程
            }
        }
        if (stoped) {
            LOGI("PERPano:awakeRunloop break");
            break;
        }
    }
}
void PERPano::resume() {
    for (int i = 0; i < decoderCount; i++) {
        decoders[i]->resume();
    }
}
void PERPano::pause() {
    for (int i = 0; i < decoderCount; i++) {
        decoders[i]->pause();
    }

}
void PERPano::stop() {
    for (int i = 0; i < decoderCount; i++) {
        decoders[i]->stop();
    }
    stoped = true;
    pthread_cond_signal(&cond);
}

bool PERPano::setStreamType(int type) {
    int width, height;
    switch (type) {
        case STREAM_TYPE_MAIN:
            width = file->binFile->width;
            height = file->binFile->height;
            break;
        case STREAM_TYPE_SUB:
            width = file->binFile->subWidth;
            height = file->binFile->subHeight;
            break;
    }
    for (int i = 0; i < decoderCount; i++) {
        int code = file->binFile->index[i];
        decoders[code]->newWidth = width;
        decoders[code]->newHeight = height;
    }
    file->streamType = type;
    return true;
}


void PERPano::decodeFrameRateStep() {
    pthread_mutex_lock(&mutex);
    decodeFrameRate++;
    pthread_mutex_unlock(&mutex);

}
int PERPano::getDecodeFrameRate() {
    pthread_mutex_lock(&mutex);
    int fr = (int)((float)decodeFrameRate / (float)file->binFile->camCount + 0.5);
    decodeFrameRate = 0;
    pthread_mutex_unlock(&mutex);
    return fr;
}
