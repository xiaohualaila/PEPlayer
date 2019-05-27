//
// Created by tir on 2016/8/30.
//

#ifndef MEDIACODEC_PESBINFILE_H
#define MEDIACODEC_PESBINFILE_H
#include "../Common.h"
#include <iostream>

#define CIF_ROW		288
#define CIF_COL		352

#define D1_ROW		576
#define D1_COL		704

#define QCIF_ROW	144
#define QCIF_COL	176

#define QVGA_ROW	240
#define QVGA_COL	320



struct PESTriCoord
{
    float x[3], y[3];				//cylinder position
    float tx[3], ty[3];				//texture position
    float sx[3], sy[3], sz[3];		//sphere position
};

struct PESBinMesh {
    int cid;
    float lambda;
    uchar imgData[MAX_ALPHA_IMAGE_SIZE];
    int fullCoordCount, subCoordCount;
    PESTriCoord * verMain[MAX_MESH_COORDINATE_COUNT] = {0};
};

struct PESBinVideoSize {
    int width;
    int height;
};

class PESBinFile {
public:
    int radius;
    char binName[64];
    int model_type, sub_model_type;
    PESBinVideoSize sub_video_size[9];
    int subWidth, subHeight;
    int camCount;
    int width, height;

    float verCoord[MAX_CAMERA_COUNT][MAX_COORDINATE_COUNT];
    float texCoord[MAX_CAMERA_COUNT][MAX_COORDINATE_COUNT];
    int verCoordCount[MAX_CAMERA_COUNT] = {0};
    int texCoordCount[MAX_CAMERA_COUNT] = {0};

    PESBinMesh * meshes[MAX_CAMERA_COUNT] = {0};
    int index[MAX_CAMERA_COUNT];
    int indexCount;
    PESBinFile();
    ~PESBinFile();
    bool openFile(char * filename);
    void setCoord();
    void setColRow();
    void flip();
};
#endif //MEDIACODEC_PESBINFILE_H
