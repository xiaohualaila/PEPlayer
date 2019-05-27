//
// Created by tir on 2016/8/15.
//

#include "PESFile.h"
#include "../MainController.h"


PESFile::PESFile() {
    fp = NULL;
    status = Closed;
    timestampIndex = 0;
    timestampCount = 0;
    timestampMap.clear();
}

PESFile::~PESFile() {
    closeFile();
}

int PESFile::readLength() {
    int len;
    fread(&len, SIZE_OF_LENGTH, 1, fp);
    return len;
}

float PESFile::readTimeStamp() {
    float ts;
    fread(&ts, SIZE_OF_TIME_STAMP, 1, fp);
    return ts;
}

bool PESFile::openFile(char *filename) {
    closeFile();
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        mc->setErrorMessage("打开Pes文件失败！");
        LOGI("PESFile: Open file faild in %s", filename);
        status = Closed;
        return false;
    } else {
        status = Opened;
        if (fseek(fp, SIZE_OF_HEAD, SEEK_SET) != 0) {
            mc->setErrorMessage("解析Pes文件失败！");
            LOGI("PESFile: Pes file seek head faild in %s", filename);
        }
        if (findIFrame()) {
            while(checkData() > 0) ;
            timestampCount = timestampIndex;
            timestampIndex = 0;
            return true;
        } else {
            fclose(fp);
            mc->setErrorMessage("解析Pes文件失败！");
            LOGI("PESFile: Can not find I Frame in %s", filename);
            return false;
        }
    }
}

bool PESFile::findIFrame() {
    while (true) {
        if (checkFrameHead()) {
            if (isIFrame()) {
                return true;
            }
            nextFrame();
        } else {
            return false;
        }
    }
}


bool PESFile::checkFrameHead() {
    uchar buffer[5]={0};
    int len = fread(buffer, 1, SIZE_OF_FRAME_HEAD, fp);
    if (len != SIZE_OF_FRAME_HEAD) {
        return false;
    }
    if ((buffer[0] == 0x00) && (buffer[1] == 0x00) && (buffer[2] == 0x00) && (buffer[3] == 0x01) && (buffer[4] == 0xFF)) {
        return true;
    } else {
        return false;
    }
}

bool PESFile::isIFrame() {
    bool result;
    uchar buffer[5] = {0};
    readTimeStamp();
    readLength();
    int len = fread(buffer, 1, SIZE_OF_FRAME_HEAD, fp);
    if (len == SIZE_OF_FRAME_HEAD) {
        if ((buffer[0] == 0x00) && (buffer[1] == 0x00) && (buffer[2] == 0x00) && (buffer[3] == 0x01)) {
            if (((buffer[4] & 0x1F) == 7)) {
                result = true;
            } else {
                result = false;
            }
        } else {
            result = false;
        }
    } else {
        result = false;
    }
    //跳回帧头
    fseek(fp, -SIZE_OF_FRAME_HEAD, SEEK_CUR);
    fseek(fp, -SIZE_OF_LENGTH, SEEK_CUR);
    fseek(fp, -SIZE_OF_TIME_STAMP, SEEK_CUR);
    fseek(fp, -SIZE_OF_FRAME_HEAD, SEEK_CUR);
    return result;
}

void PESFile::nextFrame() {
    int len;
    checkFrameHead();
    readTimeStamp();
    len = readLength();
    fseek(fp, len, SEEK_CUR);
}

void PESFile::closeFile() {
    if (fp != NULL) {
        fclose(fp);
    }
    status = Closed;
    timestampMap.clear();
    timestampIndex = 0;
    timestampCount = 0;
}
int PESFile::checkData() {
    int fpPos = ftell(fp);
    if (checkFrameHead()) {
        float ts = readTimeStamp();
        int len = readLength();
        tsList[timestampIndex++] = ts;
        timestampMap[ts] = fpPos;
        fseek(fp, len, SEEK_CUR);
        if (feof(fp)) {
            return -1;
        } else {
            return len;
        }
    } else {
        return -1;
    }
}

int PESFile::setData(uchar * buffer, int size) {
    if (status == Opened) {
        fseek(fp, timestampMap[tsList[timestampIndex++]], SEEK_SET);
        if (checkFrameHead()) {
            readTimeStamp();
            int len = readLength();
            fread(dataBuffer, len, 1, fp);
            memcpy(buffer, dataBuffer, len);
            if (feof(fp)) {
                closeFile();
                return -1;
            } else {
                return len;
            }
        }
    }
    return -1;
}

float PESFile::jumpTo(float ts) {
    int index = -1;
    if (tsList[0] > ts) {
        index = 0;
    } else {
        for (int i = 0; i < timestampCount - 1 - 1; i++) {
            if ((tsList[i] <= ts) && tsList[i + 1] > ts) {
                index = i;
            }
        }
        if (index == -1) {
            index = timestampCount - 1;
        }
    }
    while (true) {
        if (index <= 0) {
            break;
        } else {
            index--;
        }
        fseek(fp, timestampMap[tsList[index]], SEEK_SET);
        fseek(fp, SIZE_OF_FRAME_HEAD, SEEK_CUR);
        if (isIFrame()) {
            break;
        }
    }
    timestampIndex = index;
    return tsList[index];
}
