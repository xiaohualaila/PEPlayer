//
// Created by tir on 2016/9/29.
//

#ifndef PANO_PERBINFILE_H
#define PANO_PERBINFILE_H
#include "../Common.h"
#include "../PES/PESBinFile.h"
class PERBinFile: public PESBinFile {
public:
    PERBinFile();
    ~PERBinFile();
    bool openData(uchar * data);
private:

};

#endif //PANO_PERBINFILE_H
