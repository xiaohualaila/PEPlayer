//
// Created by tir on 2016/10/8.
//
#include "OnlinePano.h"
#include "../MainController.h"

static int writeFlag = true;
static void RealDataCallBack(LONG lRealHandle, const char *szSerialNo, int nStreamID, LPPE_AVFRAME_DATA lpData, void *pUserData) {
    if (mc->onlinePano != NULL) {
//        if(writeFlag){
//            LOGI("GLManager: cid %d:\n", lpData->byChannel);
//            if (lpData->lImageWidth!=0){
//                char filename[100];
//                sprintf(filename, "/storage/emulated/0/Pano/pgm/a_%d.pgm", lpData->byChannel);
//                LOGI("GLManager: 文件名 %s:\n", filename);
//                FILE *fp = fopen(filename, "wb+");
//                if (fp){
//                    LOGI("GLManager: width %d，height %d:\n", lpData->lImageWidth,lpData->lImageHeight);
//                    char buff[512];
//                    sprintf(buff, "P5\n%d %d\n%d\n", lpData->lImageWidth, lpData->lImageHeight, 255);
//                    fwrite(buff, 1, strlen(buff), fp);
//                    fwrite(lpData->pszData, 1, lpData->lImageWidth * lpData->lImageHeight, fp);
//                    fclose(fp);
//                }
//                writeFlag = false;
//            }
//        }

        LOGI("OLPano:回调 数据长度 %d,通道:%d\n", lpData->lDataLength,lpData->byChannel);
        mc->onlinePano->realDataCallBack(lpData);
    }
}

OnlinePano::OnlinePano() {
    camIndex = -1;
    binFile = NULL;
    lLoginHandle = -1;
    lRealHandle = -1;
    lFileHandle = -1;
    pthread_mutex_init(&mutex,NULL);/* 互斥锁初始化 */
    PE_Client_Init();
}

OnlinePano::~OnlinePano() {
    unregistCam();
}

bool OnlinePano::login(char *ip, int port, char *username, char *password) {
    lLoginHandle = PE_Client_Login(ip, port, username, password);
    if (lLoginHandle >= 0) {
        PE_Client_GetSadpInfoList(lLoginHandle, &SadpInfoList);
        LOGI("OLPano: Login success!");
        return true;
    } else {
        int errorcode = PE_Client_GetLastError() - 0x8000F000;
        switch (errorcode) {
            case 11:
                mc->setErrorMessage("用户名或密码错误");
                break;
            case 12:
                mc->setErrorMessage("网络错误");
                break;
            case 13:
                mc->setErrorMessage("连接超时");
                break;
        }
        LOGI("OLPano: Login failed %d", errorcode);
        return false;
    }
}

void OnlinePano::logout() {
    PE_Client_Logout(lLoginHandle);
}

int OnlinePano::getCamCount() {
    return SadpInfoList.wSadpNum;
}

char * OnlinePano::getCamNameWithIndex(int index) {
    return SadpInfoList.struSadpInfo[index].sDevName;
}

bool OnlinePano::registCamWithIndex(int index) {
    lastVideoTime = 0;
    if (index >= 0 && index < getCamCount()) {
        char * name = SadpInfoList.struSadpInfo[index].sSerialNo;
        char path[128] = "/storage/emulated/0/Pano/.online/";
        char filename[128] = "/storage/emulated/0/Pano/.online/";
        strcat(filename, name);
        strcat(filename, ".bin");
        if (binFile != NULL) {
            delete binFile;
        }
        binFile = new PESBinFile();
        if (binFile->openFile(filename) == false) {
            LOGI("OLPano: openFile failed: %s", filename);
            remove(filename);
            char url[128] = "BIN://";
            strcat(url, name);
            strcat(url, ".bin");
            lFileHandle = PE_Client_GetFileByName(lLoginHandle, url, path);
            if (lFileHandle >= 0){
                LOGI("OLPano: Download file start url %s", url);
                int pos = 0;
                while (pos != 100 && pos >= 0){
                    pos = PE_Client_GetDownloadPos(lFileHandle);
                    usleep(100000);
                }
                LOGI("OLPano: Download file done url %s", url);
                PE_Client_StopGetFile(lFileHandle);
            }
            lFileHandle = -1;
            if (binFile->openFile(filename) == false) {
                LOGI("OLPano: Open bin file faild in %s.", filename);
                return false;
            }
        }
        LOGI("OLPano: Open bin file success in %s.", filename);
        int camCount = binFile->camCount;
        if (camCount < MIN_CAMERA_COUNT && camCount > MAX_CAMERA_COUNT) {
            mc->setErrorMessage("不支持的视频分块数！");
            LOGI("OLPano: Not avaliable camera count in %d.", camCount);
            return false;
        }
        int width = binFile->width;
        int height = binFile->height;
        for (int i = 0; i < binFile->camCount; i++) {
            int code = binFile->index[i];
            OnlineDecoder * decoder = new OnlineDecoder();
            if (decoder->setDecoder(code, width, height) == true) {
                decoder->run();
                decoders[code] = decoder;
            } else {
                return false;
            }
        }
        LOGI("OLPano: Set %d decoders success!", camCount);
        mc->glm->onlinePano = this;
        mc->glm->panoType = PANO_TYPE_ONLINE;
        camIndex = index;
        setStreamType(STREAM_TYPE_MAIN);
        return true;
    }
    return false;
}


bool OnlinePano::setStreamType(int type) {
    if (lRealHandle != -1) {
        PE_Client_StopRealPlay(lRealHandle);
        lRealHandle = -1;
    }
    int width, height;
    switch (type) {
        case STREAM_TYPE_MAIN:
            width = binFile->width;
            height = binFile->height;
            break;
        case STREAM_TYPE_SUB:
            width = binFile->subWidth;
            height = binFile->subHeight;
            break;
    }
    for (int i = 0; i < binFile->camCount; i++) {
        int code = binFile->index[i];
        decoders[code]->newWidth = width;
        decoders[code]->newHeight = height;
    }
    PE_CLIENT_PREVIEWINFO PreviewInfo;
    PreviewInfo.lChannel = camIndex;
    PreviewInfo.dwStreamType = type;
    PreviewInfo.dwLinkMode = 0;
    PreviewInfo.byProtoType = 0;
    LOGI("OLPano: camIndex:%d,dwStreamType:%d!", camIndex,type);

    lRealHandle = PE_Client_RealPlay(lLoginHandle, &PreviewInfo, RealDataCallBack, (void*)this);

    return true;
}

void OnlinePano::unregistCam() {
    PE_Client_StopRealPlay(lRealHandle);
    mc->glm->panoType = PANO_TYPE_NONE;
    mc->glm->onlinePano = NULL;
    mc->glm->glmDeinit();
    if (binFile != NULL) {
        for (int i = 0; i < binFile->camCount; i++) {
            if (decoders[i] != NULL) {
                decoders[i]->stop();
                delete decoders[i];
                decoders[i] = NULL;
            }
        }
        delete binFile;
        binFile = NULL;
    }
}

void OnlinePano::realDataCallBack(LPPE_AVFRAME_DATA lpData) {
    LPPE_AVFRAME_DATA lpData2;
    PE_Client_Demux(lpData->pszData, lpData->lDataLength, &lpData2);
    int code = lpData2->byChannel;
    OnlineDecoder * decoder = decoders[code];
    int len = lpData2->lDataLength;
    uchar * data = new uchar[len];
    memcpy(data, lpData2->pszData, len);
    long long time = lpData2->lTimeStamp / 1000;
    int width = lpData2->lImageWidth;
    int height = lpData2->lImageHeight;
    unsigned int id = lpData2->lSequenceId;
    PERPackage * package = new PERPackage(len, data, time, width, height, id);
    LOGI("realDataCallBack");
//        if(writeFlag){
//            LOGI("OnlinePano: cid %d:\n", lpData->byChannel);
////            LOGI("OnlinePano: sid %d",lpData->lSequenceId);
//            if (lpData->lImageWidth!=0){
//                char filename[100];
//                sprintf(filename, "/storage/emulated/0/Pano/pgm/a_%d.pgm", lpData->byChannel);
//                LOGI("OnlinePano: 文件名 %s:\n", filename);
//                FILE *fp = fopen(filename, "wb+");
//                if (fp){
//                    LOGI("OnlinePano: width %d，height %d:\n", lpData->lImageWidth,lpData->lImageHeight);
//                    char buff[512];
//                    sprintf(buff, "P5\n%d %d\n%d\n", lpData->lImageWidth, lpData->lImageHeight, 255);
//                    fwrite(buff, 1, strlen(buff), fp);
//                    fwrite(lpData->pszData, 1, lpData->lImageWidth * lpData->lImageHeight, fp);
//                    fclose(fp);
//                }
//                writeFlag = false;
//            }
//        }

    decoder->pushBuffer(package,lpData->byFrameType == 1);
}

void OnlinePano::decodeFrameRateStep() {
    pthread_mutex_lock(&mutex);
    decodeFrameRate++;
    pthread_mutex_unlock(&mutex);

}
int OnlinePano::getDecodeFrameRate() {
    pthread_mutex_lock(&mutex);
    int fr = (int)((float)decodeFrameRate / (float)binFile->camCount + 0.5);
    decodeFrameRate = 0;
    pthread_mutex_unlock(&mutex);
    return fr;
}

int OnlinePano::getFileDownloadingPos() {
    if (lFileHandle >= 0) {
        return PE_Client_GetDownloadPos(lFileHandle);
    } else {
        return 0;
    }
}

long long OnlinePano::getCurrentVideoTime() {
    return lastVideoTime;
}

