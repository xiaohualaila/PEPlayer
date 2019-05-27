//
// Created by tir on 2016/9/29.
//
#include "PERDecoder.h"
#include "../MainController.h"

static void runDecoderRunLoop(void *arg) {
    int code = *(int *)arg;
    mc->perPano->decoders[code]->runLoop();
}

PERDecoder::PERDecoder() {
    newWidth = -1;
    newHeight = -1;
    isWaitingIFrame = true;
    decoder = NULL;
    info = NULL;
    lastTS = -1024;
    isAvailableToDisplay = false;
    isNeedJump = false;
    status = Closed;
    newStatus = Closed;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&condMutex,NULL);
    pthread_mutex_init(&queueMutex,NULL);
}

PERDecoder::~PERDecoder() {
    stop();
    if (decoder != NULL) {
        AMediaCodec_stop(decoder);
        AMediaCodec_delete(decoder);
    }
    if (info != NULL) {
        delete info;
    }
    while (!stepQueue.empty()) {
        delete stepQueue.front();
        stepQueue.pop();
    }
    LOGI("PERDec:Decoder %d deinit.", decoderCode);
}

bool PERDecoder::setDecoder(int code, int width, int height) {
    decoderCode = code;
    return setDecoder(width, height);
}

bool PERDecoder::setDecoder(int width, int height) {
    if (info == NULL) {
        info = new AMediaCodecBufferInfo();
    }
    if (decoder != NULL) {
        AMediaCodec_stop(decoder);
        AMediaCodec_delete(decoder);
        decoder = NULL;
    }
    decoder = AMediaCodec_createDecoderByType("video/avc");
    AMediaFormat * format = AMediaFormat_new();
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, "video/avc");
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, height);
    //AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, AIMAGE_FORMAT_YUV_420_888);//指定待解码的YUV格式
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, 19);//指定待解码的YUV格式
    if (AMediaCodec_configure(decoder, format, NULL, NULL, 0) == 0) {
        AMediaCodec_start(decoder);
        if (newStatus == Closed) {
            newStatus = ReadyToPlay;
        }
        return true;
    } else {
        mc->setErrorMessage("注册解码器失败！\n播放器不支持此设备！");
        LOGI("PERDec: Create mediacodec failed.");
        return false;
    }
}


void PERDecoder::pause() {
    if (newStatus == Playing) {
        newStatus = Paused;
    }
}

void PERDecoder::resume() {
    if (newStatus == Paused) {
        newStatus = Playing;
    }
}

void PERDecoder::stop() {
    newStatus = Stoped;
    pthread_cond_signal(&cond);
}


void PERDecoder::run() {
    if (newStatus == ReadyToPlay) {
        newStatus = Playing;
    }
    LOGI("PERDec:run %d runDecoderRunLoop", decoderCode);
    pthread_create(&thread, NULL, (void * (*)(void *))&runDecoderRunLoop, (void *)&decoderCode);
}

void PERDecoder::runLoop() {
    LOGI("PERDec:Decoder %d start runloop.", decoderCode);
    while (true) {

        status = newStatus;
        if (status == Paused || stepQueue.empty()) {

            LOGI("PERDec:Decoder %d wait runLoop!", decoderCode);
            pthread_cond_wait(&cond, &condMutex);                           //解码线程等待唤醒
        }

        status = newStatus;
        if (status == Stoped) {
            LOGI("PERDec:Decoder %d status is Stoped then break runLoop!", decoderCode);
            break;
        } else if (status == Playing) {
            LOGI("PERDec:Decoder %d status is Playing in runLoop!", decoderCode);
            pthread_mutex_lock(&queueMutex);
            if (newWidth != -1 || newHeight != -1) {
                setDecoder(newWidth, newHeight);
                newWidth = -1;
                newHeight = -1;
                isNeedJump = true;
                LOGI("PERDec:Decoder %d newWidth is -1!", decoderCode);
            }
            if (isNeedJump) {
                isAvailableToDisplay = false;
                clearQueue();
                isWaitingIFrame = true;
                isNeedJump = false;
                LOGI("PERDec:Decoder %d isNeedJump is true!", decoderCode);
            }
            if (step() == true) {

                popBuffer();
                mc->decodeFrameRateStep();
            }
            pthread_mutex_unlock(&queueMutex);
        }
        status = newStatus;
        if (status == Stoped) {
            break;
            LOGI("PERDec:Decoder %d break runLoop!", decoderCode);
        }
    }
    LOGI("PERDec:Decoder %d break runloop.", decoderCode);
}
static bool writeFlag = true;
bool PERDecoder::step() {
    if (stepQueue.empty()) {
        LOGI("PERDec:Decoder %d stepQueue is empty!", decoderCode);
        return false;
    }
    PERPackage * package = stepQueue.front();
    ssize_t inIndex = AMediaCodec_dequeueInputBuffer(decoder, 100000);
    if (inIndex >= 0) {
        size_t capacity;
        uint8_t *buffer = AMediaCodec_getInputBuffer(decoder, inIndex, &capacity);
        int size = package->len;
        memcpy(buffer, package->data, size);
        AMediaCodec_queueInputBuffer(decoder, inIndex, 0, size, 0, 0);
    } else {
        AMediaCodec_queueInputBuffer(decoder, inIndex, 0, 0, 0, 0);
        LOGI("PERDec:Decoder %d return false!", decoderCode);
        return false;
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

//        if(writeFlag){
////            LOGI("PERDecoder: cid :\n", decoderCode);
//            if (package->width!=0){
//                char filename[100];
//                sprintf(filename, "/storage/emulated/0/Pano/pgm/a_dec_out.pgm");
//                LOGI("PERDecoder: 文件名 %s:\n", filename);
//                FILE *fp = fopen(filename, "wb+");
//                if (fp){
//                    LOGI("PERDecoder: width %d，height %d:\n", package->width,package->height);
//                    char buff[512];
//                    sprintf(buff, "P5\n%d %d\n%d\n", package->width, package->height, 255);
//                    fwrite(buff, 1, strlen(buff), fp);
//                    fwrite(data, 1, package->width * package->height, fp);
//                    fclose(fp);
//                }
//                writeFlag = false;
//            }
//        }

            memcpy(texData, data, capacity);
            texWidth = package->width;
            texHeight = package->height;
            isAvailableToDisplay = true;
            AMediaCodec_releaseOutputBuffer(decoder, outIndex, true);
            LOGI("PERDec:Decoder %d end with return true!", decoderCode);
            return true;
    }
    LOGI("PERDec:Decoder %d end with return false!", decoderCode);
    return false;
}

void PERDecoder::pushBuffer(PERPackage *package) {
    pthread_mutex_lock(&queueMutex);
    if (isIFrame(package->data)) {
        LOGI("pushBuffer");
        isWaitingIFrame = false;
        clearQueue();
        stepQueue.push(package);
        LOGI("PERDec:pushBuffer唤醒解码线程:%d ", decoderCode);
        pthread_cond_signal(&cond);    //唤醒解码线程
    } else {
        if (isWaitingIFrame) {
//            LOGI("PERDec:pushBuffer:%d 等待i幀...", decoderCode);
            delete package;
        } else {
            stepQueue.push(package);
//            LOGI("PERDec:pushBuffer唤醒解码线程:%d ", decoderCode);
            pthread_cond_signal(&cond);    //唤醒解码线程
        }
    }
    pthread_mutex_unlock(&queueMutex);
}

void PERDecoder::pushBuffer(PERPackage * package, bool isIFrame) {
    pthread_mutex_lock(&queueMutex);
    if (isIFrame) {
        isWaitingIFrame = false;
        clearQueue();
        stepQueue.push(package);
        pthread_cond_signal(&cond);    //唤醒解码线程
    } else {
        if (isWaitingIFrame) {
            delete package;
        } else {
            stepQueue.push(package);
            pthread_cond_signal(&cond);    //唤醒解码线程
        }
    }
    pthread_mutex_unlock(&queueMutex);
}


bool PERDecoder::popBuffer() {
    if (stepQueue.empty()) {
        return false;
    } else {
        delete stepQueue.front();
        stepQueue.pop();
        return true;
    }
}

void PERDecoder::clearQueue() {
    while (!stepQueue.empty()) {
        delete stepQueue.front();
        stepQueue.pop();
    }

}

bool PERDecoder::isIFrame(uchar *data) {
    if ((data[0] == 0x00) && (data[1] == 0x00) && (data[2] == 0x00) && (data[3] == 0x01) && ((data[4] & 0x1F) == 7)) {
        LOGI("PERDec:isIFrame return true:%d ", decoderCode);
        return true;
    } else {
        LOGI("PERDec:isIFrame return flase:%d ", decoderCode);
        return false;
    }
}