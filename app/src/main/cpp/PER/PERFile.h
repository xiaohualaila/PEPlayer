//
// Created by tir on 2016/9/28.
//

#ifndef PANO_PERFILE_H
#define PANO_PERFILE_H
#include "../timeb.h"
#include "zip/ZipLibUtil.h"
#include "PERBinFile.h"

#pragma pack(push,1)
struct PERFileHead {
    char head[20];
    uchar version;
    LONG streamOffset;
    LONG streamSize;
    char packetType;
    LONG devInfoOffset;
    LONG recordInfoOffset;
};

struct PERDevHead {
    unsigned char head; //0x40
    char devName[256];
    char serialNum[64];
    LONG binZipOffset;
    LONG binZipSize;
    int  streamMode;
};

struct PERRecordHead {
    uchar	head; //0x41
    timeb	createTime;
    timeb	startTime;
    timeb	endTime;
    LONG	listOffset;//绝对偏移量
    LONG	listSize;
    LONG	listCellSize;
};

struct PERIndexCell{
    uchar	head;
    LONG	preCellOffset;			//相对于index_list_order,负数为无效
    LONG	nextCellOffset;			//相对于index_list_order,负数为无效
    LONG	streamPackageOffset;	//相对于stream_load_order,负数为无效
    timeb	currentTime;
    float	cPDt;
    unsigned	isIFrame: 8;
    unsigned	streamID: 8;
    ulong	frameID;				//当前帧ID
    ulong	keyframeID;				//当前帧依赖的关键帧ID
};
struct PERPackageHead {
    unsigned head;
    LONG pesHeadOffset;
};

struct PERPackagePESHead {
    unsigned head : 24;
    unsigned streamID : 8;
    LONG videoHeadOffset;
};

struct PERPackageVideoHead {
    unsigned head;				//0x000001B3
    timeb    tPTS;				//UTS绝对时间
    float    fPTS;				//相对于文件开始时间
    timeb    tDTS;				//UTS绝对时间
    float    fDTS;				//相对于文件开始时间
    LONG     videoDataOffset;	//距离这个头的偏移
    LONG     videoDataSize;
    //扩展区
    unsigned isIFrame;			//关键帧
    ulong frameID;				//当前帧ID
    ulong IFrameID;				//当前帧依赖的关键帧ID
};

struct PERVideoPackage {
    PERPackageHead head;
    PERPackagePESHead pesHead;
    PERPackageVideoHead videoHead;
};
#pragma pack(pop)

class PERFile {
public:
    int streamType;
    FILE * file;
    PERFileHead head;
    PERDevHead devHead;
    PERRecordHead recordHead;
    PERIndexCell indexList[MAX_CAMERA_COUNT][MAX_PER_CELL_COUNT];
    int indexIndex[MAX_CAMERA_COUNT] = {0};
    PERBinFile * binFile;
    PERFile();
    ~PERFile();
    bool openFile(char * filename);
    void seekTo(int offset);
    bool setVideoData(int * code);

};
#endif //PANO_PERFILE_H
