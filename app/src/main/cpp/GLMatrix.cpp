//
// Created by tir on 2016/8/26.
//

#include "GLMatrix.h"

GLMatrix::GLMatrix() {

}

int GLMatrix::getIndex(int row, int col) {
    return row * 4 + col;
}
void GLMatrix::mMakeVec3(float *result, float x, float y, float z) {
    result[0] = x;
    result[1] = y;
    result[2] = z;
}

void GLMatrix::mLogi(float *matrix) {
    LOGI("{");
    LOGI("%f, %f, %f, %f", matrix[0], matrix[1], matrix[2], matrix[3]);
    LOGI("%f, %f, %f, %f", matrix[4], matrix[5], matrix[6], matrix[7]);
    LOGI("%f, %f, %f, %f", matrix[8], matrix[9], matrix[10], matrix[11]);
    LOGI("%f, %f, %f, %f", matrix[12], matrix[13], matrix[14], matrix[15]);
    LOGI("}");
}

void GLMatrix::mMakeUnit(float *matrix) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                matrix[getIndex(i, j)] = 1;
            } else {
                matrix[getIndex(i, j)] = 0;
            }
        }
    }
}

void GLMatrix::mMakeEmpty(float *matrix) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[getIndex(i, j)] = 0;
        }
    }
}

void GLMatrix::mMakeCopy(float *result, float *source) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result[getIndex(row, col)] = source[getIndex(row, col)];
        }
    }
}

void GLMatrix::mCalMulti(float *result, float *m1, float *m2) {
    mMakeEmpty(result);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result[getIndex(j, i)] += m1[getIndex(k, i)] * m2[getIndex(j, k)];
            }
        }
    }
}

void GLMatrix::mTranslate(float *matrix, float * vec3) {
    for (int col = 0; col < 3; col ++) {
        matrix[getIndex(3, col)] = matrix[getIndex(0, col)] * vec3[col]
                                   + matrix[getIndex(1, col)] * vec3[col]
                                   + matrix[getIndex(2, col)] * vec3[col]
                                   + matrix[getIndex(3, col)];
    }
}

void GLMatrix::mRotate(float *matrix, float angle, float * vec3) {
    float rotate[16];
    float axis[3];
    float temp[3];
    float c = cos(angle);
    float s = sin(angle);
    for (int col = 0; col < 3; col++) {
        axis[col] = vec3[col];
    }
    for (int col = 0; col < 3; col++) {
        temp[col] = (1 - c) * axis[col];
    }
    rotate[getIndex(0, 0)] = c + temp[0] * axis[0];
    rotate[getIndex(0, 1)] = 0 + temp[0] * axis[1] + s * axis[2];
    rotate[getIndex(0, 2)] = 0 + temp[0] * axis[2] - s * axis[1];

    rotate[getIndex(1, 0)] = 0 + temp[1] * axis[0] - s * axis[2];
    rotate[getIndex(1, 1)] = c + temp[1] * axis[1];
    rotate[getIndex(1, 2)] = 0 + temp[1] * axis[2] + s * axis[0];

    rotate[getIndex(2, 0)] = 0 + temp[2] * axis[0] + s * axis[1];
    rotate[getIndex(2, 1)] = 0 + temp[2] * axis[1] - s * axis[0];
    rotate[getIndex(2, 2)] = c + temp[2] * axis[2];

    float mCopy[16];
    mMakeCopy(mCopy, matrix);
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            matrix[getIndex(row, col)] = mCopy[getIndex(0, col)] * rotate[getIndex(row, 0)]
                                       + mCopy[getIndex(1, col)] * rotate[getIndex(row, 1)]
                                       + mCopy[getIndex(2, col)] * rotate[getIndex(row, 2)];
        }
    }
}

void GLMatrix::mFrustum(float * matrix, float left, float right, float bottom, float top, float zNear, float zFar) {
    mMakeUnit(matrix);
    matrix[getIndex(0, 0)] = (2 * zNear) / (right - left);
    matrix[getIndex(1, 1)] = (2 * zNear) / (top - bottom);
    matrix[getIndex(2, 0)] = (right + left) / (right - left);
    matrix[getIndex(2, 1)] = (top + bottom) / (top - bottom);
    matrix[getIndex(2, 2)] = -(zFar + zNear) / (zFar - zNear);
    matrix[getIndex(2, 3)] = -1;
    matrix[getIndex(3, 2)] = -(2 * zFar * zNear) / (zFar - zNear);
}

