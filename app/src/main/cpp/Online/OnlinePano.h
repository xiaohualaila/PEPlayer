//
// Created by tir on 2016/10/8.
//

#ifndef PANO_ONLINEPANO_H
#define PANO_ONLINEPANO_H

#include "../Common.h"
#include "../OnlineSDK/PEClientSDK/PEClientSDK.h"
#include "OnlineDecoder.h"
#include "../PES/PESBinFile.h"

class OnlinePano {
public:
    int camIndex;
    OnlineDecoder *decoders[MAX_CAMERA_COUNT] = {0};
    PESBinFile * binFile = NULL;
    long long lastVideoTime;
    long lLoginHandle;
    long lRealHandle;
    int lFileHandle;
    PE_CLIENT_SADPINFO_LIST SadpInfoList;
    OnlinePano();
    ~OnlinePano();
    bool login(char * ip, int port, char * username, char * password);
    void logout();
    int getCamCount();
    char * getCamNameWithIndex(int index);
    bool registCamWithIndex(int index);
    bool setStreamType(int type);
    void unregistCam();
    void realDataCallBack(LPPE_AVFRAME_DATA lpData);
    void decodeFrameRateStep();
    int getDecodeFrameRate();
    int getFileDownloadingPos();
    long long getCurrentVideoTime();
private:
    int decodeFrameRate;
    pthread_mutex_t mutex;// pthread_mutex_t 是Linux下的互斥锁
};
#endif //PANO_ONLINEPANO_H
