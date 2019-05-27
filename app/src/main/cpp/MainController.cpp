//
// Created by tir on 2016/9/1.
//
#include "MainController.h"

MainController * MainController::instance;

MainController* MainController::getInstance() {
    if (instance == NULL )
        instance = new MainController();
    return instance;
}

MainController::MainController() {
    pesPano = NULL;
    perPano = NULL;
    onlinePano = NULL;
    glm = new GLManager();
    panoType = PANO_TYPE_NONE;
    timeline = 0;
    surfaceIndex = 0;
    lastErrorMessage = "No Error.";
    LOGI("MC: Init.");
}

void MainController::setErrorMessage(char *msg) {
    lastErrorMessage = msg;
}

bool MainController::registPano(char * binFilename, int type) {
    if (panoType != PANO_TYPE_NONE) {
        unregistPano();
    }
    panoType = type;
    timeline = 0;
    switch (type) {
        case PANO_TYPE_PES:
            pesPano = new PESPano();
            if (pesPano->setPano(binFilename) == true) {
                LOGI("MC: Regist PES pano success.");
                return true;
            } else {
                delete pesPano;
                pesPano = NULL;
                panoType = -1;
                LOGI("MC: Regist PES pano failed.");
                return false;
            }
        case PANO_TYPE_PER:
            perPano = new PERPano();
            if (perPano->setPano(binFilename) == true) {
                LOGI("MC: Regist PER pano success.");
                return true;
            } else {
                delete perPano;
                perPano = NULL;
                panoType = -1;
                LOGI("MC: Regist PER pano failed.");
                return false;
            }
        default:
            panoType = -1;
            return false;
    }
}

bool MainController::registPano(char *ip, int port, char *username, char *password) {
    onlinePano = new OnlinePano();
    if (onlinePano->login(ip, port, username, password) == true) {
        panoType = PANO_TYPE_ONLINE;
        return true;
    } else {
        panoType = PANO_TYPE_NONE;
        delete onlinePano;
        return false;
    }
}

bool MainController::registPanoToGLM() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return false;
            } else {
                glm->panoType = panoType;
                glm->pesPano = pesPano;
                return true;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return false;
            } else {
                glm->panoType = panoType;
                glm->perPano = perPano;
                return true;
            }
        default:
            return false;
    }
}

void MainController::unregistPano() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano != NULL) {
                pesPano->stop();
                glm->glmDeinit();
                delete pesPano;
            }
            break;
        case PANO_TYPE_PER:
            if (perPano != NULL) {
                perPano->stop();
                glm->glmDeinit();
                delete perPano;
            }
            break;
        case PANO_TYPE_ONLINE:
            if (onlinePano != NULL) {
                onlinePano->logout();
                glm->glmDeinit();
                delete onlinePano;
            }
            break;
        default:
            break;
    }
    glm->panoType = PANO_TYPE_NONE;
    glm->pesPano = NULL;
    glm->perPano = NULL;
    panoType = PANO_TYPE_NONE;
    timeline = 0;
    perPano = NULL;
    pesPano = NULL;
}

bool MainController::runPano() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return false;
            } else {
                pesPano->run();
                return true;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return false;
            } else {
                perPano->run();
                return true;
            }
        default:
            return false;
    }
}

float MainController::jumpPanoToTimeline() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return 0;
            } else {
                return pesPano->jumpToTimeline();
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return 0;
            } else {
                return perPano->jumpToTimeline();
            }
        default:
            return 0;
    }
}

bool MainController::awakeDecoders() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return false;
            } else {
                pesPano->awakeDecoders();
                return true;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return false;
            } else {
                perPano->awakeDecoders();
                return true;
            }
        default:
            return false;
    }
}

bool MainController::resumePano() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return false;
            } else {
                pesPano->resume();
                return true;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return false;
            } else {
                perPano->resume();
                return true;
            }
        default:
            return false;
    }
}
bool MainController::pausePano() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return false;
            } else {
                pesPano->pause();
                return true;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return false;
            } else {
                perPano->pause();
                return true;
            }
        default:
            return false;
    }
}
bool MainController::stopPano() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return false;
            } else {
                pesPano->stop();
                return true;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return false;
            } else {
                perPano->stop();
                return true;
            }
        default:
            return false;
    }
}

bool MainController::decodeFrameRateStep() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return false;
            } else {
                pesPano->decodeFrameRateStep();
                return true;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return false;
            } else {
                perPano->decodeFrameRateStep();
                return true;
            }
        case PANO_TYPE_ONLINE:
            if (onlinePano == NULL) {
                return false;
            } else {

                onlinePano->decodeFrameRateStep();
                return true;
            }
        default:
            return false;
    }
}

float MainController::getTotalDuration() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return -1;
            } else {
                return pesPano->totalDuration;
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return -1;
            } else {
                return perPano->totalDuration;
            }
        default:
            return 0;
    }
}

int MainController::getDecodeFrameRate() {
    switch (panoType) {
        case PANO_TYPE_PES:
            if (pesPano == NULL) {
                return 0;
            } else {
                return pesPano->getDecodeFrameRate();
            }
        case PANO_TYPE_PER:
            if (perPano == NULL) {
                return 0;
            } else {
                return perPano->getDecodeFrameRate();
            }
        case PANO_TYPE_ONLINE:
            if (onlinePano == NULL) {
                return 0;
            } else {
                return onlinePano->getDecodeFrameRate();
            }
        default:
            return 0;
    }
}