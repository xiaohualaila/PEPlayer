//
// Created by tir on 2016/9/28.
//

#include "PERFile.h"
#include "../MainController.h"

PERFile::PERFile() {
    streamType = STREAM_TYPE_MAIN;
    file = NULL;
}

PERFile::~PERFile() {
    if (file != NULL) {
        fclose(file);
    }
    delete binFile;
}

bool PERFile::openFile(char *filename) {
    file = fopen(filename, "rb");
    fread(&head, sizeof(PERFileHead), 1, file);
    fseek(file, head.devInfoOffset, SEEK_SET);
    fread(&devHead, sizeof(PERDevHead), 1, file);
    fseek(file, head.recordInfoOffset, SEEK_SET);
    fread(&recordHead, sizeof(PERRecordHead), 1, file);
    fseek(file, recordHead.listOffset, SEEK_SET);
    for (int i = 0; i < recordHead.listSize; i++) {
        PERIndexCell cell;
        fread(&cell, recordHead.listCellSize, 1, file);
        int index = cell.streamID - 0xE0;
        if ((index >= 0) && (index < MAX_CAMERA_COUNT)) {
            indexList[index][indexIndex[index]++] = cell;
        }
    }
    fseek(file, devHead.binZipOffset, SEEK_SET);
    size_t binZipSize = devHead.binZipSize;
    uchar * binZipData = new uchar[binZipSize];
    fread(binZipData, binZipSize, 1, file);
    uchar * binData;
    size_t binSize;
    int result = ExtracMemory2Memory(binZipData, binZipSize, &binData, &binSize);
    binFile = new PERBinFile();
    binFile->openData(binData);
    delete[] binZipData;
    return true;
}

void PERFile::seekTo(int offset) {
    if (file == NULL) {
        return;
    }
    fseek(file, offset, SEEK_SET);
}

bool PERFile::setVideoData(int * code) {
    if (file == NULL) {
        return false;
    }
    int sizeOfHead = sizeof(PERPackageHead) + sizeof(PERPackagePESHead) + sizeof(PERPackageVideoHead);
    while (true) {
        PERVideoPackage package;
        fread(&package.head, sizeof(PERPackageHead), 1, file);
        fread(&package.pesHead, sizeof(PERPackagePESHead), 1, file);
        fread(&package.videoHead, sizeof(PERPackageVideoHead), 1, file);
        int id = -1;
        int width, height;
        switch (streamType) {
            case STREAM_TYPE_MAIN:
                id = package.pesHead.streamID - 0xE0;
                width = binFile->width;
                height = binFile->height;
                break;
            case STREAM_TYPE_SUB:
                id = package.pesHead.streamID - 0xB0;
                width = binFile->subWidth;
                height = binFile->subHeight;
                break;
        }
        int size = package.videoHead.videoDataSize;
        float ts = package.videoHead.fPTS;
        if ((id >= 0) && (id < MAX_CAMERA_COUNT)) {
            PERDecoder * decoder = mc->perPano->decoders[id];
            if ((ts <= mc->timeline) && (ts >= decoder->lastTS)) {
                uchar * data = new uchar[size];
                fread(data, size, 1, file);
                PERPackage * package = new PERPackage(size, data, ts, width, height, 0);
                decoder->pushBuffer(package);
                *code = id;
                return true;
            } else if ((ts <= mc->timeline) && (ts < decoder->lastTS)) {
                fseek(file, size, SEEK_CUR);                //跳过
            } else if ((ts > mc->timeline) && (ts >= decoder->lastTS)) {
                fseek(file, -sizeOfHead, SEEK_CUR);         //跳回去
            } else if ((ts > mc->timeline) && (ts < decoder->lastTS)) {
                fseek(file, size, SEEK_CUR);                //跳过
            }
            return false;
        } else {
            fseek(file, size, SEEK_CUR);
        }
        if (feof(file)) {
            return false;
        }
    }
}