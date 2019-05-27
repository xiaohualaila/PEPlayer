//
// Created by tir on 2016/9/29.
//

#ifndef PANO_PERPANO_H
#define PANO_PERPANO_H
#include "../Common.h"
#include "PERDecoder.h"
#include "PERFile.h"

class PERPano {
public:
    bool stoped;
    bool isNeedJumpToTimeline;
    LONG jumpOffset;
    int decoderCount;
    float totalDuration;
    PERFile * file;
    PERDecoder * decoders[MAX_CAMERA_COUNT] = {0};
    PERPano();
    ~PERPano();
    bool setPano(char * filename);
    void run();
    float jumpToTimeline();
    void awakeDecoders();
    void awakeRunloop();
    void resume();
    void pause();
    void stop();
    bool setStreamType(int type);
    void decodeFrameRateStep();
    int getDecodeFrameRate();
private:
    int decodeFrameRate;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_mutex_t condMutex;

};
#endif //PANO_PERPANO_H
