//
// Created by tir on 2016/9/1.
//

#ifndef PANO_MAINCONTROLLER_H
#define PANO_MAINCONTROLLER_H
#include <unistd.h>
#include "Common.h"
#include "GLManager.h"
#include "PES/PESPano.h"
#include "PER/PERPano.h"
#include "Online/OnlinePano.h"

class MainController {
public:
    int panoType;
    PESPano * pesPano;
    PERPano * perPano;
    OnlinePano * onlinePano;
    GLManager * glm;
    float timeline;
    char * lastErrorMessage;
    int surfaceIndex;
    void setErrorMessage(char * msg);
    static MainController * getInstance();
    bool registPano(char * binFilename, int type);
    bool registPano(char *ip, int port, char *username, char *password);
    bool registPanoToGLM();
    void unregistPano();
    bool runPano();
    float jumpPanoToTimeline();
    bool awakeDecoders();
    bool resumePano();
    bool pausePano();
    bool stopPano();
    bool decodeFrameRateStep();
    float getTotalDuration();
    int getDecodeFrameRate();
private:
    static MainController * instance;
    MainController();
};
#endif //PANO_MAINCONTROLLER_H
