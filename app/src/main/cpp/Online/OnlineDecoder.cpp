//
// Created by tir on 2016/10/9.
//
#include "OnlineDecoder.h"
#include "../MainController.h"

static void runDecoderRunLoop(void *arg) {
    int code = *(int *)arg;
    mc->onlinePano->decoders[code]->runLoop();
}

OnlineDecoder::OnlineDecoder() {

}

OnlineDecoder::~OnlineDecoder() {

}

void OnlineDecoder::run() {
    if (newStatus == ReadyToPlay) {
        newStatus = Playing;
    }
    pthread_create(&thread, NULL, (void * (*)(void *))&runDecoderRunLoop, (void *)&decoderCode);
}

bool OnlineDecoder::step() {
    bool result = PERDecoder::step();
    if (result == true) {
        PERPackage * package = stepQueue.front();
        mc->onlinePano->lastVideoTime = package->timestamp;
    }
    return result;
}

