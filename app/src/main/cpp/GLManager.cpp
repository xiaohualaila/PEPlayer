//
// Created by tir on 2016/9/1.
//

#include "GLManager.h"
#include "MainController.h"


GLManager::GLManager() {
    GLMatrix::mMakeUnit(glProjectionMatrix);
    GLMatrix::mMakeVec3(glRotateXAxis, 1, 0, 0);
    GLMatrix::mMakeVec3(glRotateYAxis, 0, 1, 0);
    GLMatrix::mMakeVec3(glRotateZAxis, 0, 0, 1);
    width = 1;
    height = 1;
    panoType = PANO_TYPE_NONE;
    pesPano = NULL;
    perPano = NULL;
    onlinePano = NULL;
}


bool GLManager::glmInit(int width, int height) {
    isVRMode = false;
    isGlobularMode = true;
    isNeedReloadBin = false;
    radius = 0;
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < MAX_CAMERA_COUNT; i++) {
        for (int j = 0; j < 4; j++) {
            glGenTextures(1, &textures[i][j]);
            glBindTexture(GL_TEXTURE_2D, textures[i][j]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glmReloadBin();
    //创建纹理
    glGenTextures(1, &displayTexture);
    glBindTexture(GL_TEXTURE_2D, displayTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    //创建renderbuffer
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //创建framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glFinish();
    //glCheckFramebufferStatus(GL_FRAMEBUFFER);

    shaderProgram = buildProgram(VERTEX_SHADER, FRAG_SHADER);
    glUseProgram(shaderProgram);
    shaderVertex = glGetAttribLocation(shaderProgram, "vPosition");
    shaderTexture = glGetAttribLocation(shaderProgram, "a_texCoord");
    shaderMatrix = glGetUniformLocation(shaderProgram, "glMatrix");
    tex_type = glGetUniformLocation(shaderProgram, "SampleType");
    tex_y = glGetUniformLocation(shaderProgram, "SamplerY");
    tex_u = glGetUniformLocation(shaderProgram, "SamplerU");
    tex_v = glGetUniformLocation(shaderProgram, "SamplerV");
    tex_a = glGetUniformLocation(shaderProgram, "SamplerA");
    return true;
}

void GLManager::glmReloadBin() {
    PESBinFile * binFile;
    switch (panoType) {
        case PANO_TYPE_PES:
            binFile = pesPano->binFile;
            break;
        case PANO_TYPE_PER:
            binFile = perPano->file->binFile;
            break;
        case PANO_TYPE_ONLINE:
            binFile = onlinePano->binFile;
            break;
        default:
            break;
    }
    for (int i = 0; i < binFile->camCount; i++) {
        int code = binFile->index[i];
        if (vboVer[code] == 0) {
            glGenBuffers(1, &vboVer[code]);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vboVer[code]);
        glBufferData(GL_ARRAY_BUFFER, binFile->verCoordCount[code] * sizeof(GLfloat), binFile->verCoord[code], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (vboTex[code] == 0) {
            glGenBuffers(1, &vboTex[code]);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vboTex[code]);
        glBufferData(GL_ARRAY_BUFFER, binFile->texCoordCount[code] * sizeof(GLfloat), binFile->texCoord[code], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, textures[code][3]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, binFile->width, binFile->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, binFile->meshes[code]->imgData);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    radius = binFile->radius;
    radius = 450;
//    setOriginalVerCoord(binFile->width, binFile->height);
//    if (radius == 0) {
//        radius = binFile->radius;
//    } else {
//        radius = -radius;
//    }
    isNeedReloadBin = false;
}


void GLManager::glmDeinit() {
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    for (int i = 0; i < MAX_CAMERA_COUNT; i++) {
        for (int j = 0; j < 4; j++) {
            glDeleteTextures(1, &textures[i][j]);
        }
        glDeleteBuffers(1,&vboVer[i]);
        glDeleteBuffers(1,&vboTex[i]);
    }
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteRenderbuffers(1, &renderBuffer);
    glDeleteTextures(1, &displayTexture);
}

void GLManager::glmResize() {
//    glmResize(width, height, fovy);
    glmResize(width, height);
}

//void GLManager::glmResize(int width, int height, float fovy) {
//    this->width = width;
//    this->height = height;
//    this->fovy = fovy;
//    GLfloat aspect;
//    if (isVRMode) {
//        aspect = (GLfloat)(width / 2)/(GLfloat)height;
//    } else {
//        aspect = (GLfloat)width/(GLfloat)height;
//    }
//    GLfloat zNear = 0.1f;
//    GLfloat zFar = 2000.0f;
//
//    GLfloat top = zNear * ((GLfloat) tan(fovy * PI / 360.0));
//    GLfloat bottom = -top;
//    GLfloat left = bottom * aspect;
//    GLfloat right = top * aspect;
//
//    GLMatrix::mFrustum(glProjectionMatrix, left, right, bottom, top, zNear, zFar);
//    glViewport(0, 0, width, height);
//}

void GLManager::glmResize(int width, int height) {
    this->width = width;
    this->height = height;
    GLfloat aspect;
    if (isVRMode) {
        aspect = (GLfloat)(width / 2)/(GLfloat)height;
    } else {
        aspect = (GLfloat)width/(GLfloat)height;
    }
    GLfloat zNear = 0.1f;
    GLfloat zFar = 2000.0f;

    GLfloat top = zNear * ((GLfloat) tan(fovy * PI / 360.0));
    GLfloat bottom = -top;
    GLfloat left = bottom * aspect;
    GLfloat right = top * aspect;

    GLMatrix::mFrustum(glProjectionMatrix, left, right, bottom, top, zNear, zFar);
    glViewport(0, 0, width, height);
}

int writeFlag = true;
void GLManager::glmStep(float x, float y, float z, float fovy) {
    if (panoType == PANO_TYPE_NONE) {
        return;
    }
    if ((panoType == PANO_TYPE_PES) && (pesPano == NULL)) {
        return;
    }
    if ((panoType == PANO_TYPE_PER) && (perPano == NULL)) {
        return;
    }
    if ((panoType == PANO_TYPE_ONLINE) && (onlinePano == NULL)) {
        return;
    }
    if (isNeedReloadBin) {
        glmReloadBin();
    }
    bool isVRMode = this->isVRMode;
    bool isGlobularMode = this->isGlobularMode;
//    if (isVRMode) {
//        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
//    }
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(GL_BACKGROUND_COLOR);
    float yd = (float)(y / 180 * PI);
    float xd = (float)(x / 180 * PI);
    float zd = (float)(z / 180 * PI);
    this->fovy = fovy;
    glmResize();
    float glModelViewMatrix[16];
    float glMatrix[16];
    GLMatrix::mMakeUnit(glModelViewMatrix);
    GLMatrix::mMakeUnit(glMatrix);
    if (isGlobularMode) {
        float glTranslate[3];
        GLMatrix::mMakeVec3(glTranslate, 0, 0, -(radius * 7 / 8));
        GLMatrix::mTranslate(glModelViewMatrix, glTranslate);
        GLMatrix::mRotate(glModelViewMatrix, -yd, glRotateXAxis);
        GLMatrix::mRotate(glModelViewMatrix, -xd, glRotateZAxis);
        float rollAxis[3] = {(float) sin(-xd), (float) cos(-xd), 0};
        GLMatrix::mRotate(glModelViewMatrix, -zd, rollAxis);
        GLMatrix::mCalMulti(glMatrix, glProjectionMatrix, glModelViewMatrix);
    } else {
        float glTranslate[3];
        GLMatrix::mMakeVec3(glTranslate, xd * 500, -yd * 500, 0);
        GLMatrix::mTranslate(glModelViewMatrix, glTranslate);
        GLMatrix::mCalMulti(glMatrix, glProjectionMatrix, glModelViewMatrix);
    }
    int width;
    int height;
    int camCount;
    PESBinFile * binFile;
    switch (panoType) {
        case PANO_TYPE_PES:
            binFile = pesPano->binFile;
            break;
        case PANO_TYPE_PER:
            binFile = perPano->file->binFile;
            break;
        case PANO_TYPE_ONLINE:
            binFile = onlinePano->binFile;
            break;
    }
    camCount = binFile->camCount;
    for (int i = 0; i < camCount; i++) {
        int code;
        bool isAvailableToDisplay = false;
        uchar *data = NULL;
        switch (panoType) {
            case PANO_TYPE_PES:
                code = pesPano->binFile->index[i];
                isAvailableToDisplay = pesPano->decoders[code]->isAvailableToDisplay;
                data = pesPano->decoders[code]->texData;
                width = pesPano->binFile->width;
                height = pesPano->binFile->height;
                break;
            case PANO_TYPE_PER:
                code = perPano->file->binFile->index[i];
                isAvailableToDisplay = perPano->decoders[code]->isAvailableToDisplay;
                data = perPano->decoders[code]->texData;
                width = perPano->decoders[code]->texWidth;
                height = perPano->decoders[code]->texHeight;
                break;
            case PANO_TYPE_ONLINE:
                code = onlinePano->binFile->index[i];
                isAvailableToDisplay = onlinePano->decoders[code]->isAvailableToDisplay;
                data = onlinePano->decoders[code]->texData;
                width = onlinePano->decoders[code]->texWidth;
                height = onlinePano->decoders[code]->texHeight;
                break;
            default:
                return;
        }

//        if(writeFlag){
//            LOGI("GLManager: cid %d:\n", code);
//            if (width!=0){
//                char filename[100];
//                sprintf(filename, "/storage/emulated/0/Pano/pgm/a_%d.pgm", code);
//                LOGI("GLManager: 文件名 %s:\n", filename);
//                FILE *fp = fopen(filename, "wb+");
//                if (fp){
//                    LOGI("GLManager: width %d，height %d:\n", width,height);
//                    char buff[512];
//                    sprintf(buff, "P5\n%d %d\n%d\n", width, height, 255);
//                    fwrite(buff, 1, strlen(buff), fp);
//                    fwrite(data, 1, width * height, fp);
//                    fclose(fp);
//                }
//                writeFlag = false;
//            }
//        }

        if (isAvailableToDisplay) {
            glBindTexture(GL_TEXTURE_2D, textures[code][0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
                         GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, textures[code][1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE,
                         GL_UNSIGNED_BYTE, data + width * height);
            glBindTexture(GL_TEXTURE_2D, textures[code][2]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE,
                         GL_UNSIGNED_BYTE, data + width * height * 5 / 4);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[code][0]);
        glUniform1i(tex_y, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[code][1]);
        glUniform1i(tex_u, 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textures[code][2]);
        glUniform1i(tex_v, 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textures[code][3]);
        glUniform1i(tex_a, 3);


        if (isGlobularMode) {
            glUniform1i(tex_type, 1);
            glUniformMatrix4fv(shaderMatrix, 1, GL_FALSE, glMatrix);
            glBindBuffer(GL_ARRAY_BUFFER, vboVer[code]);
            glVertexAttribPointer(shaderVertex, 3, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray(shaderVertex);

            glBindBuffer(GL_ARRAY_BUFFER, vboTex[code]);
            glVertexAttribPointer(shaderTexture, 2, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray(shaderTexture);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            switch (panoType) {
                case PANO_TYPE_PES:
                    glDrawArrays(GL_TRIANGLES, 0, pesPano->binFile->verCoordCount[code]);
                    break;
                case PANO_TYPE_PER:
                    glDrawArrays(GL_TRIANGLES, 0, perPano->file->binFile->verCoordCount[code]);
                    break;
                case PANO_TYPE_ONLINE:
                    if(isVRMode)
                    {
                        //width = 2880;
                        //height = 1600;
                        LOGI("ttttt:width:%d,height:%d",this->width,this->height);
                        glViewport(0,0,this->width/2,this->height);
                        glDrawArrays(GL_TRIANGLES, 0, onlinePano->binFile->verCoordCount[code]);
                        glViewport(this->width/2,0,this->width/2,this->height);
                        glDrawArrays(GL_TRIANGLES, 0, onlinePano->binFile->verCoordCount[code]);

                    } else
                    {
                        glDrawArrays(GL_TRIANGLES, 0, onlinePano->binFile->verCoordCount[code]);

                    }

                    break;
                default:
                    return;
            }
        } else {
            glUniform1i(tex_type, 2);
            glUniformMatrix4fv(shaderMatrix, 1, GL_FALSE, glMatrix);
            glVertexAttribPointer(shaderVertex, 3, GL_FLOAT, 0, 0, originalVerCoord[i]);
            glEnableVertexAttribArray(shaderVertex);

            glVertexAttribPointer(shaderTexture, 2, GL_FLOAT, 0, 0, TEXTURE_COORD);
            glEnableVertexAttribArray(shaderTexture);
            switch (panoType) {
                case PANO_TYPE_PES:
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    break;
                case PANO_TYPE_PER:
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    break;
                case PANO_TYPE_ONLINE:
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    break;
                default:
                    return;
            }
        }
    }

//    if (isVRMode) {
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        glBindTexture(GL_TEXTURE_2D, displayTexture);
//        glGenerateMipmap(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, 0);
//
//        glClear(GL_COLOR_BUFFER_BIT);
//        glClearColor(GL_BACKGROUND_COLOR);
//
//        glUniform1i(tex_type, 0);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, displayTexture);
//        glUniform1i(tex_y, 0);
//        GLMatrix::mMakeUnit(glMatrix);
//        glUniformMatrix4fv(shaderMatrix, 1, GL_FALSE, glMatrix);
//        glVertexAttribPointer(shaderVertex, 2, GL_FLOAT, 0, 0, VR_VERTEX_COORD);
//        glEnableVertexAttribArray(shaderVertex);
//        glVertexAttribPointer(shaderTexture, 2, GL_FLOAT, 0, 0, TEXTURE_COORD);
//        glEnableVertexAttribArray(shaderTexture);
//        glDrawArrays(GL_TRIANGLES, 0, 12);
//
//        glBindTexture(GL_TEXTURE_2D, 0);
//
//    }

}

GLuint GLManager::buildShader(const char* source, GLenum shaderType) {
    GLuint shaderHandle = glCreateShader(shaderType);
    if (shaderHandle) {
        glShaderSource(shaderHandle, 1, &source, 0);
        glCompileShader(shaderHandle);
        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*)malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shaderHandle, infoLen, NULL, buf);
                    mc->setErrorMessage("编译Shader失败！");
                    LOGI("GLM: Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shaderHandle);
                shaderHandle = 0;
            }
        }
    }
    return shaderHandle;
}

GLuint GLManager::buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint programHandle = glCreateProgram();
    if (programHandle) {
        glAttachShader(programHandle, vertexShader);
        glAttachShader(programHandle, fragmentShader);
        glLinkProgram(programHandle);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
                    mc->setErrorMessage("链接Shader失败！");
                    LOGI("GLM: Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(programHandle);
            programHandle = 0;
        }
    }
    return programHandle;
}

void GLManager::setOriginalVerCoord(int w, int h) {
    float z = -500;
    float fh = 250 * w / h / 2;
    for (int i = 0; i < 8; i++) {
        float pointA[3] = {500.0f * (i - 1), fh, z};
        float pointB[3] = {500.0f * i, fh, z};
        float pointC[3] = {500.0f * (i - 1), -fh, z};
        float pointD[3] = {500.0f * i, -fh, z};
        int c = 0;
        float * index[6] = {pointA, pointB, pointD, pointA, pointD, pointC};
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 3; k++) {
                 originalVerCoord[i][c++] = index[j][k];
            }
        }
    }
}