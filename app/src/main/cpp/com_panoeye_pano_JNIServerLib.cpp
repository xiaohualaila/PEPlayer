//
// Created by tir on 2016/9/1.
//

#include "com_panoeye_pano_JNIServerLib.h"
#include "MainController.h"
extern "C" {
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_glmInit(JNIEnv * env , jobject obj, jint width, jint height ) {
        mc->registPanoToGLM();
        mc->glm->glmInit(width, height);
    }
//    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_glmResize(JNIEnv * env, jobject obj, jint width, jint height, jfloat fovy) {
//        mc->glm->glmResize(width, height, fovy);
//    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_glmResize(JNIEnv * env, jobject obj, jint width, jint height) {
        mc->glm->glmResize(width, height);
    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_glmStep(JNIEnv * env, jobject obj, jfloat gldx, jfloat gldy, jfloat gldz, jfloat fovy) {
        mc->glm->glmStep(gldx, gldy, gldz, fovy);
    }
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_registPano(JNIEnv * env, jobject obj, jstring filename, jint panoType) {
        char * _filename;
        _filename = (char*)env->GetStringUTFChars(filename,0);
        bool result;
        switch (panoType) {
        case PANO_TYPE_PES:
            result = mc->registPano(_filename, panoType);
            break;
        case PANO_TYPE_PER:
            result = mc->registPano(_filename, panoType);
            break;
        default: result = false;
            break;
        }
        env->ReleaseStringUTFChars(filename, _filename);
        return result;
    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_unregistPano(JNIEnv * env, jobject obj) {
        mc->unregistPano();
    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_setTimeline(JNIEnv * env, jobject obj, jfloat timeline) {
        mc->timeline = timeline;
    }
    JNIEXPORT jfloat JNICALL Java_com_panoeye_peplayer_JNIServerLib_getTotalDuration(JNIEnv * env, jobject obj) {
        return mc->getTotalDuration();
    }
    JNIEXPORT jint JNICALL Java_com_panoeye_peplayer_JNIServerLib_getDecodeFrameRate(JNIEnv * env, jobject obj) {
        return mc->getDecodeFrameRate();
    }
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_awakeDecoders(JNIEnv * env, jobject obj) {
        return mc->awakeDecoders();
    }
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_runPano(JNIEnv * env, jobject obj) {
        return mc->runPano();
    }
    JNIEXPORT jfloat JNICALL Java_com_panoeye_peplayer_JNIServerLib_jumpPanoToTimeline(JNIEnv * env, jobject obj) {
        return mc->jumpPanoToTimeline();
    }
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_resumePano(JNIEnv * env, jclass obj) {
        return mc->resumePano();
    }
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_pausePano(JNIEnv * env, jobject obj) {
        return mc->pausePano();
    }
    JNIEXPORT jstring JNICALL Java_com_panoeye_peplayer_JNIServerLib_getLastErrorMessage(JNIEnv * env, jobject obj) {
        return env->NewStringUTF(mc->lastErrorMessage);
    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_setMode(JNIEnv * env, jobject obj, jint mode) {
        switch (mode) {
        case PANO_TYPE_PES:
            mc->glm->isVRMode = false;
            break;
        case PANO_TYPE_PER:
            mc->glm->isVRMode = true;
            break;
        case PANO_TYPE_ONLINE:
            mc->glm->isVRMode = true;
            break;
        default:
            mc->glm->isVRMode = false;
            break;
        }
        mc->glm->glmResize();
    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_flip(JNIEnv * env, jobject obj) {
        switch (mc->panoType) {
            case PANO_TYPE_PES:
                mc->pesPano->binFile->flip();
                break;
            case PANO_TYPE_PER:
                mc->perPano->file->binFile->flip();
                break;
            case PANO_TYPE_ONLINE:
                mc->onlinePano->binFile->flip();
            break;
            default:
                return;
            }
        mc->glm->isNeedReloadBin = true;
    }
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_setStreamType(JNIEnv * env, jobject obj, jint type) {
        switch (mc->panoType) {
        case PANO_TYPE_PES:
            return mc->pesPano->setStreamType(type);
        case PANO_TYPE_PER:
            return mc->perPano->setStreamType(type);
        case PANO_TYPE_ONLINE:
            return mc->onlinePano->setStreamType(type);
        default:
            return false;
        }
    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_setVideoMode(JNIEnv * env, jobject obj, jint mode) {
        switch (mode) {
        case 0:
            mc->glm->isGlobularMode = true;
            break;
        case 1:
            mc->glm->isGlobularMode = false;
            break;
        }
    }

    //
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_registOnlinePano(JNIEnv * env, jobject obj, jstring ip, jint port, jstring username, jstring password) {
        char * _ip;
        _ip = (char*)env->GetStringUTFChars(ip,0);
        char * _username;
        _username = (char*)env->GetStringUTFChars(username,0);
        char * _password;
        _password = (char*)env->GetStringUTFChars(password,0);
        bool result = mc->registPano(_ip, port, _username, _password);
        env->ReleaseStringUTFChars(ip, _ip);
        env->ReleaseStringUTFChars(username, _username);
        env->ReleaseStringUTFChars(password, _password);
        return result;
    }

//一键生成
//JNIEXPORT jboolean JNICALL
//Java_com_panoeye_peplayer_JNIServerLib_registOnlinePano(JNIEnv *env, jclass type, jstring ip_,
//                                                        jint port, jstring username_,
//                                                        jstring password_) {
//    const char *ip = env->GetStringUTFChars(ip_, 0);
//    const char *username = env->GetStringUTFChars(username_, 0);
//    const char *password = env->GetStringUTFChars(password_, 0);
//
//    // TODO
//    bool result = mc->registPano(const_cast<char *>(ip), port, (char*)username, (char*)password);
//
//    env->ReleaseStringUTFChars(ip_, ip);
//    env->ReleaseStringUTFChars(username_, username);
//    env->ReleaseStringUTFChars(password_, password);
//    return result;
//}

    JNIEXPORT jint JNICALL Java_com_panoeye_peplayer_JNIServerLib_getOnlinePanoCamCount(JNIEnv * env, jobject obj) {
        switch (mc->panoType) {
        case PANO_TYPE_ONLINE:
            return mc->onlinePano->getCamCount();
        default:
            return 0;
        }
    }
    JNIEXPORT jbyteArray JNICALL Java_com_panoeye_peplayer_JNIServerLib_getOnlinePanoCamNameWithIndex(JNIEnv * env, jobject obj, jint index) {
        char str[128] = "unknow";
        switch (mc->panoType) {
        case PANO_TYPE_ONLINE:
        {
            char * name = mc->onlinePano->getCamNameWithIndex(index);
            memcpy(str, name, strlen(name));
            break;
        }
        default:
            break;
        }
        jbyte *by = (jbyte*)str;
        jbyteArray jarray = env->NewByteArray(strlen(str));
        env->SetByteArrayRegion(jarray,0,strlen(str),by);
        return jarray;
    }
    JNIEXPORT jboolean JNICALL Java_com_panoeye_peplayer_JNIServerLib_registOnlinePanoCamWithIndex(JNIEnv * env, jobject obj, jint index) {
        switch (mc->panoType) {
        case PANO_TYPE_ONLINE: {
            bool result = mc->onlinePano->registCamWithIndex(index);
            if (result == false) {
                mc->onlinePano->unregistCam();
            }
            return result;
        }
        default:
            return false;
        }
    }
    JNIEXPORT void JNICALL Java_com_panoeye_peplayer_JNIServerLib_unregistOnlinePanoCam(JNIEnv * env, jobject obj) {
        switch (mc->panoType) {
        case PANO_TYPE_ONLINE:
            return mc->onlinePano->unregistCam();
            break;
        default:
            break;
        }
    }
    JNIEXPORT jlong JNICALL Java_com_panoeye_peplayer_JNIServerLib_getOnlinePanoCurrentTimeStamp(JNIEnv * env, jobject obj) {
        switch (mc->panoType) {
        case PANO_TYPE_ONLINE:
            //LOGI("time %ld", mc->onlinePano->getCurrentVideoTime());
            return mc->onlinePano->getCurrentVideoTime();
        default:
            return 0;
        }
    }
    JNIEXPORT jint JNICALL Java_com_panoeye_peplayer_JNIServerLib_getOnlinePanoFileDownloadingPos(JNIEnv * env, jobject obj) {
        switch (mc->panoType) {
        case PANO_TYPE_ONLINE:
            return mc->onlinePano->getFileDownloadingPos();
        default:
            return 0;
        }
    }
}