//
// Created by tir on 2016/10/10.
//

#include "PERPackage.h"

PERPackage::PERPackage(int _len, uchar *_data, long long _timestamp, int _width, int _height, int _id) {
    len = _len;
    data = _data;
    timestamp = _timestamp;
    width = _width;
    height = _height;
    id = _id;
}

PERPackage::~PERPackage() {
    delete[] data;
}