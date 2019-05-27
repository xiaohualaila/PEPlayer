//
// Created by tir on 2016/8/15.
//

#ifndef MEDIACODEC_PESFILE_H
#define MEDIACODEC_PESFILE_H

#include <iostream>
#include <string>
#include <map>
#include "../Common.h"


class PESFile {
    enum PESFileStatus {
        Opened,
        Closed,
    };
public:
    PESFileStatus status;
    float tsList[MAX_TIME_STAMP_SIZE];
    std::map<float ,int> timestampMap;      //时间戳：数据在文件中的位置
    int timestampIndex;                     //扫描时用作数组填充，播放时用作帧数据索引
    int timestampCount;
    PESFile();
    ~PESFile();
    bool openFile(char * filename);
    void closeFile();
    int checkData();
    float jumpTo(float TS);
    int setData(uchar * buffer, int size);
private:
    FILE * fp;
    uchar * dataBuffer[MAX_BUFFER_SIZE] = {0};
    bool checkFrameHead();
    float readTimeStamp();
    int readLength();
    bool isIFrame();
    bool findIFrame();
    void nextFrame();
};
#endif //MEDIACODEC_PESFILE_H
