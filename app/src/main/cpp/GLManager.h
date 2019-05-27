//
// Created by tir on 2016/9/1.
//

#ifndef PANO_GLMANAGER_H
#define PANO_GLMANAGER_H
#include <math.h>
#include <cstring>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Common.h"
#include "GLMatrix.h"
#include "PES/PESPano.h"
#include "PER/PERPano.h"
#include "Online/OnlinePano.h"

#define GL_BACKGROUND_COLOR 0.8f, 0.8f, 0.8f, 1.0f


class GLManager {
public:
    GLManager();
    int panoType;
    PESPano * pesPano;
    PERPano * perPano;
    OnlinePano * onlinePano;
    float radius;
    bool isVRMode = false;
    bool isGlobularMode = true;
    bool isNeedReloadBin = false;
    bool glmInit(int width, int height);
    void glmReloadBin();
    void glmDeinit();
    void glmResize();
//    void glmResize(int width, int height, float fovy);
    void glmResize(int width, int height);
    void glmStep(float x, float y, float z, float fovy);
private:
    float width, height, fovy;
    GLuint vertexShader, fragmentShader;
    GLuint shaderProgram;
    GLuint shaderVertex, shaderTexture, shaderMatrix;
    GLuint tex_y, tex_u, tex_v, tex_a, tex_type;
    GLuint vboVer[MAX_CAMERA_COUNT] = {0}, vboTex[MAX_CAMERA_COUNT] = {0};
    float glRotateXAxis[3], glRotateYAxis[3], glRotateZAxis[3];
    float glProjectionMatrix[16];
    GLuint textures[MAX_CAMERA_COUNT][4];
    GLuint renderBuffer, frameBuffer, displayTexture;
    GLfloat originalVerCoord[8][18];
    void setOriginalVerCoord(int w, int h);
    GLuint buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
    GLuint buildShader(const char* source, GLenum shaderType);
};

static const char* VERTEX_SHADER =
                "attribute vec4 vPosition;                      \n"
                "attribute vec2 a_texCoord;                     \n"
                "uniform mat4 glMatrix;                         \n"
                "varying vec2 tc;                               \n"
                "void main()                                    \n"
                "{                                              \n"
                "   gl_Position =  glMatrix * vPosition;        \n"
                "   tc = a_texCoord;                            \n"
                "}                                              \n";

static const char* FRAG_SHADER =
                "varying lowp vec2 tc;                              \n"
                "uniform int SampleType;                            \n"//0 = rgb, 1 = yuv球面， 2 = yuv平面
                "uniform sampler2D SamplerY;                        \n"
                "uniform sampler2D SamplerU;                        \n"
                "uniform sampler2D SamplerV;                        \n"
                "uniform sampler2D SamplerA;                        \n"
                "void main(void)                                    \n"
                "{                                                  \n"
                "   if (SampleType == 0) {                          \n"
                "       gl_FragColor = vec4(texture2D(SamplerY, tc).r,\n"
                "                           texture2D(SamplerY, tc).g,\n"
                "                           texture2D(SamplerY, tc).b, 1);\n"
                "   } else {                                        \n"
                "       mediump vec3 yuv;                           \n"
                "       lowp vec3 rgb;                              \n"
                "       yuv.x = texture2D(SamplerY, tc).r;          \n"
                "       yuv.y = texture2D(SamplerU, tc).r - 0.5;    \n"
                "       yuv.z = texture2D(SamplerV, tc).r - 0.5;    \n"
                "       rgb = mat3( 1,        1,       1,           \n"
                "                    0,    -0.39465,  2.03211,      \n"
                "                 1.13983, -0.58060,     0 ) * yuv; \n"
                "       if (SampleType == 1) {                      \n"
                "           gl_FragColor = vec4(rgb, texture2D(SamplerA, tc).r);\n"
                "       } else {                                    \n"
                "           gl_FragColor = vec4(rgb, 1);            \n"
                "       }                                           \n"
                "   }                                               \n"
                "}                                                  \n";
static GLfloat VR_VERTEX_COORD[] = {
        -1, -1,
        0, -1,
        0, 1,
        -1, -1,
        0, 1,
        -1, 1,

        0, -1,
        1, -1,
        1, 1,
        0, -1,
        1, 1,
        0, 1,
};
static GLfloat TEXTURE_COORD[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 0,
        1, 1,
        0, 1,

        0, 0,
        1, 0,
        1, 1,
        0, 0,
        1, 1,
        0, 1,
};

#endif //PANO_GLMANAGER_H
