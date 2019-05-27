//
// Created by tir on 2016/10/7.
//

#ifndef PANO_TIMEB_H
#define PANO_TIMEB_H
#include <sys/time.h>
#include <chrono>

struct timeb {
    time_t          time;
    unsigned short  millitm;
    short           timezone;
    short           dstflag;
};

static int ftime(timeb*  timebuf) {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(now - std::chrono::system_clock::from_time_t(0));
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - std::chrono::system_clock::from_time_t(sec.count()));
    timebuf->time = sec.count();
    timebuf->millitm = ms.count();
    timebuf->timezone = 0;
    timebuf->dstflag = 0;
}
#endif //PANO_TIMEB_H
