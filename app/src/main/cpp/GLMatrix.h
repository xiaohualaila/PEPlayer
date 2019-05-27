//
// Created by tir on 2016/8/26.
//

#ifndef MEDIACODEC_GLMATRIX_H
#define MEDIACODEC_GLMATRIX_H
#include <math.h>
#include "Common.h"

class GLMatrix {
public:
    GLMatrix();
    static void mCalMulti(float * result, float * m1, float * m2);
    static void mMakeVec3(float * result, float x, float y, float z);
    static void mMakeCopy(float * result, float * source);
    static void mLogi(float * matrix);
    static void mMakeUnit(float * matrix);
    static void mMakeEmpty(float * matrix);
    static void mTranslate(float * matrix, float * vec3);
    static void mRotate(float * matrix, float angle, float * vec3);
    static void mFrustum(float * matrix, float left, float right, float bottom, float top, float zNear, float zFar);
private:
    static int getIndex(int row, int col);
};

#endif //MEDIACODEC_GLMATRIX_H
