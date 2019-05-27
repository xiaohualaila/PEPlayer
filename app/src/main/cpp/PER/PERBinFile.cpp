//
// Created by tir on 2016/9/29.
//
#include "PERBinFile.h"
#include "../MainController.h"

PERBinFile::PERBinFile() : PESBinFile() {
}

PERBinFile::~PERBinFile() {

}

bool PERBinFile::openData(uchar *data) {
    int dataIndex = 0;
    dataIndex += 264;
    memcpy(&radius, data + dataIndex, sizeof(int));
    long off = 256 + sizeof(int)* 7 + 16 + sizeof(char)* 4 + 64;
    dataIndex = off;
    memcpy(&binName, data + dataIndex, 64);
    model_type = binName[0] - 'A';
    sub_model_type = binName[1] - '0';
    long off1 = off + 64 + 16 * sizeof(int)+16 * sizeof(char)+11 * sizeof(int)+10 * sizeof(float)+916;
    dataIndex = off1;
    dataIndex = off1;
    memcpy(&camCount, data + dataIndex, sizeof(int));
    dataIndex += sizeof(int);
    if (camCount > MAX_CAMERA_COUNT || camCount < MIN_CAMERA_COUNT) {
        mc->setErrorMessage("不支持的Bin数据！");
        LOGI("PERBinFile: Open data faild!");
        return false;
    }
    for (int i = 0; i < camCount; i++) {
        PESBinMesh * mesh = new PESBinMesh();
        memcpy(&mesh->cid, data + dataIndex, sizeof(int));
        dataIndex += sizeof(int);
        memcpy(&mesh->lambda, data + dataIndex, sizeof(int));
        dataIndex += sizeof(int);
        dataIndex += sizeof(int);
        int row, col;
        memcpy(&row, data + dataIndex, sizeof(int));
        dataIndex += sizeof(int);
        memcpy(&col, data + dataIndex, sizeof(int));
        dataIndex += sizeof(int);
        if (col * row >= MAX_ALPHA_IMAGE_SIZE) {
            mc->setErrorMessage("不支持的Bin数据！");
            LOGI("PERBinFile: Alpha image is too large!");
            return false;
        }
        width = col;
        height = row;
        dataIndex += 568;
        memcpy(mesh->imgData, data + dataIndex, col * row);
        dataIndex += col * row;
        int fullSize, subSize;
        memcpy(&fullSize, data + dataIndex, sizeof(int));
        dataIndex += sizeof(int);
        memcpy(&subSize, data + dataIndex, sizeof(int));
        dataIndex += sizeof(int);
        mesh->fullCoordCount = fullSize;
        mesh->subCoordCount = subSize;
        for (int j = 0; j < fullSize; j++) {
            PESTriCoord * tri = new PESTriCoord();
            mesh->verMain[j] = tri;
            memcpy(tri, data + dataIndex, sizeof(PESTriCoord));
            dataIndex += sizeof(PESTriCoord);
        }
        data += subSize * sizeof(PESTriCoord);
        //pes里的mesh是按照cid，per里是按照顺序。
        meshes[i] = mesh;
        index[indexCount++] = i;
    }
    setCoord();
    return true;
}
