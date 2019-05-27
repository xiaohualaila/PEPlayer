//
// Created by tir on 2016/7/29.
//
#include "PESDecoder.h"
#include "../MainController.h"

static void runDecoderRunLoop(void *arg) {
    int code = *(int *)arg;
    mc->pesPano->decoders[code]->runLoop();
}

PESDecoder::PESDecoder() {
    decoder = NULL;
    info = NULL;
    isAvailableToDisplay = false;
    status = Closed;
    newStatus = Closed;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&condMutex,NULL);
}

PESDecoder::~PESDecoder() {
    if (decoder != NULL) {
        AMediaCodec_stop(decoder);
        AMediaCodec_delete(decoder);
    }
    if (info != NULL) {
        delete info;
    }
    if (pesFile != NULL) {
        delete pesFile;
    }
    LOGI("PESDec:Decoder %d deinit.", decoderCode);
}

bool PESDecoder::setFile(char *sample, int code, int width, int height) {
    decoderCode = code;
    pesFile = new PESFile();
    if (pesFile->openFile(sample) == true) {
        info = new AMediaCodecBufferInfo();
        decoder = AMediaCodec_createDecoderByType("video/avc");
        AMediaFormat * format = AMediaFormat_new();
        AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, "video/avc");
        AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, width);
        AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, height);
        if (AMediaCodec_configure(decoder, format, NULL, NULL, 0) == 0) {
            AMediaCodec_start(decoder);
            newStatus = ReadyToPlay;
            return true;
        } else {
            mc->setErrorMessage("注册解码器失败！\n播放器不支持此设备！");
            LOGI("PESDec: Create mediacodec failed.");
            return false;
        }
    } else {
        LOGI("PESDec: Open pes file failed in code %d file %s.", decoderCode, sample);
        return false;
    }
}

void PESDecoder::pause() {
    if (newStatus == Playing) {
        newStatus = Paused;
    }
}

void PESDecoder::resume() {
    if (newStatus == Paused) {
        newStatus = Playing;
    }
}

void PESDecoder::stop() {
    newStatus = Stoped;
    pthread_cond_signal(&cond);
    pesFile->closeFile();
}

void PESDecoder::run() {
    jumpTo(0);
    if (newStatus == ReadyToPlay) {
        newStatus = Playing;
    }
    pthread_create(&thread, NULL, (void * (*)(void *))&runDecoderRunLoop, (void *)&decoderCode);
}


void PESDecoder::runLoop() {
    LOGI("PESDec:Decoder %d start runloop.", decoderCode);
    jumpTo(0);
    while (true) {
        status = newStatus;
        if (status == Playing) {
            if (pesFile->timestampIndex < pesFile->timestampCount - 1 - 1) {
                if (mc->timeline >= pesFile->tsList[pesFile->timestampIndex]) {
                    step();
                    mc->decodeFrameRateStep();
                }
            }
        }
        pthread_cond_wait(&cond, &condMutex);                           //解码线程等待唤醒
        if (status == Stoped) {
            break;
        }
    }
    LOGI("PESDec:Decoder %d break runloop.", decoderCode);
}

float PESDecoder::jumpTo(float ts) {
    return pesFile->jumpTo(ts);
}

void PESDecoder::step() {
    LOGI("PECLIENT:step");
    ssize_t inIndex = AMediaCodec_dequeueInputBuffer(decoder, 100000);
    if (inIndex >= 0) {
        size_t capacity;
        uint8_t *buffer = AMediaCodec_getInputBuffer(decoder, inIndex, &capacity);
        int length = setData(buffer, capacity);
        if (length >= 0) {
            AMediaCodec_queueInputBuffer(decoder, inIndex, 0, length, 0, 0);
        } else {
            AMediaCodec_queueInputBuffer(decoder, inIndex, 0, 0, 0, 0);
            return;
        }
    } else {
        AMediaCodec_queueInputBuffer(decoder, inIndex, 0, 0, 0, 0);
        return;
    }
    ssize_t outIndex = AMediaCodec_dequeueOutputBuffer(decoder, info, 100000);
    switch (outIndex) {
        case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
            break;
        case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED:
            AMediaCodec_getOutputFormat(decoder);
            break;
        case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
            break;
        default:
            size_t capacity = 0;
            uchar * data = AMediaCodec_getOutputBuffer(decoder, outIndex, &capacity);
            memcpy(texData, data, capacity);
            isAvailableToDisplay = true;
            AMediaCodec_releaseOutputBuffer(decoder, outIndex, true);
            break;
    }
    if ((info->flags && AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) != 0) {
        newStatus = Stoped;
        LOGI("Decoder %d decode done", decoderCode);
    }
}


int PESDecoder::setData(uint8_t * buffer, size_t size) {
    return pesFile->setData(buffer, size);
}
