//
// Created by tir on 2016/7/29.
//

#ifndef MEDIACODEC_JNIPLAYTHREAD_H
#define MEDIACODEC_JNIPLAYTHREAD_H

#include "../Common.h"
#include "PESFile.h"
#include <media\NdkMediaCodec.h>
#include <media\NdkMediaExtractor.h>
#include <pthread.h>


class PESDecoder
{
    enum PESDecoderStatus {
        ReadyToPlay,
        Playing,
        Paused,
        Stoped,
        Closed,
    };
public:
    int decoderCode;
    PESFile * pesFile;
    bool isAvailableToDisplay;
    uchar texData[MAX_ALPHA_IMAGE_SIZE];
    PESDecoderStatus status;    //只在runLoop里修改
    PESDecoderStatus newStatus;
    pthread_cond_t cond;
    pthread_mutex_t condMutex;
    PESDecoder();
    ~PESDecoder();
    bool setFile(char * sample, int code, int width, int height);
    void run();         //条件：ReadyToPlay
    void runLoop();
    float jumpTo(float ts);
    void pause();       //条件：Playing
    void resume();      //条件：Paused
    void stop();        //条件：ReadyToPlay, Playing, Paused
private:
    pthread_t thread;
    AMediaCodec *decoder;
    AMediaCodecBufferInfo *info;
    int setData(uint8_t * buffer, size_t size);
    void step();
};


#endif //MEDIACODEC_JNIPLAYTHREAD_H
