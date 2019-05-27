//
// Created by tir on 2016/9/29.
//

#ifndef PANO_PERDECODER_H
#define PANO_PERDECODER_H
#include <media\NdkMediaCodec.h>
#include <media\NdkMediaExtractor.h>
#include <pthread.h>
#include "../Common.h"
#include "PERPackage.h"
#include <queue>

class PERDecoder {
public:
    enum PERDecoderStatus {
        ReadyToPlay,
        Playing,
        Paused,
        Stoped,
        Closed,
    };
    pthread_t thread;
    AMediaCodec *decoder;
    AMediaCodecBufferInfo *info;
public:
    int decoderCode;
    bool isAvailableToDisplay;
    bool isNeedJump;
    bool isWaitingIFrame;
    uchar texData[MAX_ALPHA_IMAGE_SIZE];
    int texWidth, texHeight;
    std::queue<PERPackage *> stepQueue;
    int newWidth, newHeight;
    float lastTS;
    PERDecoderStatus status;    //只在runLoop里修改
    PERDecoderStatus newStatus;
    pthread_cond_t cond;
    pthread_mutex_t condMutex;
    pthread_mutex_t queueMutex;
    PERDecoder();
    ~PERDecoder();
    bool setDecoder(int code, int width, int height);
    bool setDecoder(int width, int height);
    virtual void run();         //条件：ReadyToPlay
    void runLoop();
    void pause();       //条件：Playing
    void resume();      //条件：Paused
    void stop();        //条件：ReadyToPlay, Playing, Paused
    virtual bool step();
    void pushBuffer(PERPackage * package);
    void pushBuffer(PERPackage * package, bool isIFrame);
    bool popBuffer();
    void clearQueue();
    bool isIFrame(uchar *data);
};

#endif //PANO_PERDECODER_H
