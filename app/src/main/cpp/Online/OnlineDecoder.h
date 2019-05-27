//
// Created by tir on 2016/10/9.
//

#ifndef PANO_ONLINEDECODER_H
#define PANO_ONLINEDECODER_H
#include "../Common.h"
#include "../PER/PERDecoder.h"
#include "../OnlineSDK/PEClientSDK/PEClientSDK.h"
#include <media\NdkMediaCodec.h>
#include <media\NdkMediaExtractor.h>
#include <queue>
#include <pthread.h>

class OnlineDecoder: public PERDecoder {
public:
    OnlineDecoder();
    ~OnlineDecoder();
    void run();
    bool step();
private:
};


#endif //PANO_ONLINEDECODER_H
