//
// Created by tir on 2016/10/10.
//

#ifndef PANO_PERPACKAGE_H
#define PANO_PERPACKAGE_H

#include "../Common.h"

class PERPackage {
public:
    int len;
    uchar * data;
    long long timestamp;
    int width, height;
    unsigned int id;
    PERPackage(int _len, uchar * _data, long long _timestamp, int _width, int _height, int _id);
    ~PERPackage();
};
#endif //PANO_PERPACKAGE_H
