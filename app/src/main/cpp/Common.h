//
// Created by tir on 2016/9/1.
//
#ifndef PANO_COMMON_H
#define PANO_COMMON_H
#include <android/log.h>
#define LOG_TAG    "JNI_LOG"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define _FILE_OFFSET_BITS 64
#define PI 3.1415926
#define LONG int
#define uchar unsigned char
#define ulong unsigned LONG
#define mc MainController::getInstance()

#define SEEK_SET 0  //文件开头
#define SEEK_CUR 1  //当前位置
#define SEEK_END 2  //文件末尾
#define MAX_BUFFER_SIZE 128 * 1024
#define MAX_TIME_STAMP_SIZE 1 * 1024 * 1024

#define SIZE_OF_HEAD 2000
#define SIZE_OF_FRAME_HEAD 5
#define SIZE_OF_TIME_STAMP 4
#define SIZE_OF_LENGTH 4

#define MAX_CAMERA_COUNT 8
#define MIN_CAMERA_COUNT 2

#define MAX_ALPHA_IMAGE_SIZE 6 * 1024 * 1024
#define MAX_MESH_COORDINATE_COUNT 4 * 1024
#define MAX_COORDINATE_COUNT 16 * 1024 *10

#define MAX_PER_CELL_COUNT 1 *1024 * 1024

#define PANO_TYPE_NONE -1
#define PANO_TYPE_PES 0
#define PANO_TYPE_PER 1
#define PANO_TYPE_ONLINE 2

//主子码流
#define STREAM_TYPE_MAIN 0
#define STREAM_TYPE_SUB 1


#endif //PANO_COMMON_H
