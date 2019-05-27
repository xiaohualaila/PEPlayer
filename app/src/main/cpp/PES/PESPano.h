//
// Created by tir on 2016/9/1.
//

#ifndef PANO_PESPANO_H
#define PANO_PESPANO_H
#include "../Common.h"
#include "PESBinFile.h"
#include "PESDecoder.h"

class PESPano {
public:
    int decoderCount;
    float totalDuration;
    PESDecoder *decoders[MAX_CAMERA_COUNT] = {0};
    PESBinFile *binFile;
    PESPano();
    ~PESPano();
    bool setPano(char * filename);
    void run();
    float jumpToTimeline();
    void awakeDecoders();
    void resume();
    void pause();
    void stop();
    bool setStreamType(int type);
    void decodeFrameRateStep();
    int getDecodeFrameRate();
    void setPESTask(char * filename, int code,int * doneThreadCountPtr, int * successCountPtr);
private:
    int decodeFrameRate;
    pthread_mutex_t mutex;
    pthread_mutex_t threadMutex;
    pthread_t thread[MAX_CAMERA_COUNT];
    bool setPES(char * filename, int code);
};
#endif //PANO_PESPANO_H
