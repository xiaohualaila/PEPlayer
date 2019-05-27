//
// Created by tir on 2016/8/30.
//

#include "PESBinFile.h"
#include "../MainController.h"

#define PE_B  1		//1: 8-ch, 标清：704x576;
#define PE_M  12	//12: 8-ch, 标清车载：M0: 704-576;高清车载 MA：1280x720

PESBinFile::PESBinFile() {

    sub_video_size[0].height = QCIF_ROW;
    sub_video_size[0].width = QCIF_COL;

    sub_video_size[1].height = QVGA_ROW;
    sub_video_size[1].width = QVGA_COL;

    sub_video_size[2].height = CIF_ROW;
    sub_video_size[2].width = CIF_COL;

    sub_video_size[3].height = 480;
    sub_video_size[3].width = 640;

    sub_video_size[4].height = 320;
    sub_video_size[4].width = 560;

    sub_video_size[5].height = 576;
    sub_video_size[5].width = 720;

    sub_video_size[6].height = D1_ROW;
    sub_video_size[6].width = D1_COL;

    sub_video_size[7].height = 360;
    sub_video_size[7].width = 640;

    sub_video_size[8].height = 1080;
    sub_video_size[8].width = 1920;

    indexCount = 0;
}

PESBinFile::~PESBinFile() {
    for (int i = 0; i < MAX_CAMERA_COUNT; i++) {
        PESBinMesh * mesh = meshes[i];
        if (mesh != NULL) {
            for (int j = 0; j < MAX_MESH_COORDINATE_COUNT; j++) {
                delete mesh->verMain[j];
            }
        }
        delete mesh;
    }
}

bool PESBinFile::openFile(char *filename) {
    FILE * pBinFile;
    pBinFile = fopen(filename, "rb");
    if (pBinFile == NULL) {
        mc->setErrorMessage("打开Bin文件失败！");
        LOGI("PESBinFile: Open file faild in %s", filename);
        return false;
    }
    fseek(pBinFile, 264, SEEK_SET);
    fread(&radius, sizeof(int), 1, pBinFile);

    long off = 256 + sizeof(int)* 7 + 16 + sizeof(char)* 4 + 64;
    fseek(pBinFile, off, SEEK_SET);
    fread(binName, 1, 64, pBinFile);
    model_type = binName[0] - 'A';
    sub_model_type = binName[1] - '0';
    long off1 = off + 64 + 16 * sizeof(int)+16 * sizeof(char)+11 * sizeof(int)+10 * sizeof(float)+916;
    fseek(pBinFile, off1, SEEK_SET);
    fread(&camCount, sizeof(int), 1, pBinFile);
    if (camCount > MAX_CAMERA_COUNT || camCount < MIN_CAMERA_COUNT) {
        mc->setErrorMessage("不支持的Bin文件！");
        LOGI("PESBinFile: Open file faild in %s", filename);
        return false;
    }
    for (int i = 0; i < camCount; i++) {
        PESBinMesh * mesh = new PESBinMesh();
        fread(&mesh->cid, sizeof(int), 1, pBinFile);
        fread(&mesh->lambda, sizeof(float), 1, pBinFile);
        fseek(pBinFile, sizeof(int), SEEK_CUR);
        int row, col;
        fread(&row, sizeof(int), 1, pBinFile);
        fread(&col, sizeof(int), 1, pBinFile);
        width = col;
        height = row;
        if (col * row >= MAX_ALPHA_IMAGE_SIZE) {
            mc->setErrorMessage("不支持的Bin文件！");
            LOGI("PESBinFile: Alpha image is too large in %s", filename);
            return false;
        }
        fseek(pBinFile, 568, SEEK_CUR);
        fread(mesh->imgData, 1, col * row, pBinFile);
        int fullSize, subSize;
        fread(&fullSize, sizeof(int), 1, pBinFile);
        fread(&subSize, sizeof(int), 1, pBinFile);
        if (fullSize >= MAX_MESH_COORDINATE_COUNT) {
            mc->setErrorMessage("不支持的Bin文件！");
            LOGI("PESBinFile: Mesh is too much in %s", filename);
            return false;
        }
        mesh->fullCoordCount = fullSize;
        mesh->subCoordCount = subSize;
        for (int j = 0; j < fullSize; j++) {
            PESTriCoord * tri = new PESTriCoord();
            mesh->verMain[j] = tri;
            fread(tri, sizeof(PESTriCoord), 1, pBinFile);
        }
        fseek(pBinFile, sizeof(PESTriCoord) * subSize, SEEK_CUR);
        meshes[mesh->cid] = mesh;
        index[indexCount++] = mesh->cid;
    }
    fclose(pBinFile);
    setCoord();
    return true;
}


void PESBinFile::setCoord() {
    int coordCount = 0;
    int texCount = 0;
    for (int i = 0; i < camCount; i++) {
        int code = index[i];
        PESBinMesh * mesh = meshes[code];
        coordCount = 0;
        texCount = 0;
        for (int j = 0; j < mesh->fullCoordCount; j++) {
            for (int k = 0; k < 3; k++) {
                verCoord[code][coordCount++] = mesh->verMain[j]->sx[k];
                verCoord[code][coordCount++] = mesh->verMain[j]->sy[k];
                verCoord[code][coordCount++] = -mesh->verMain[j]->sz[k];
                texCoord[code][texCount++] = mesh->verMain[j]->tx[k];
                texCoord[code][texCount++] = mesh->verMain[j]->ty[k];
            }
        }
        verCoordCount[code] = coordCount;
        texCoordCount[code] = texCount;
        if (coordCount > MAX_COORDINATE_COUNT) {
            LOGI("!!!stub!");
        }
    }
    setColRow();
}

void PESBinFile::setColRow() {
    subWidth = sub_video_size[2].width;// CIF_COL
    subHeight = sub_video_size[2].height;// CIF_ROW
    switch (model_type)
    {
        case PE_B:
            subWidth = sub_video_size[0].width;// QCIF_COL
            subHeight = sub_video_size[0].height;// QCIF_ROW
            break;
        case PE_M:
            switch (sub_model_type)
            {
                case 0:
                    subWidth = sub_video_size[0].width;// QCIF_COL
                    subHeight = sub_video_size[0].height;// QCIF_ROW
                    break;
            }
            break;
    }
}

void PESBinFile::flip() {
    for (int i = 0; i < camCount; i++) {
        int code = index[i];
        PESBinMesh * mesh = meshes[code];
        for (int j = 0; j < MAX_COORDINATE_COUNT; j++) {
            if (j % 3 == 2) {
                verCoord[code][j] = -verCoord[code][j];
            }
        }
    }
}