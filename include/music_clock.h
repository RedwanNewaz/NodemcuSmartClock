#pragma once 

#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2SNoDAC.h>
#include "azan_sound.h"

class MusicClock: public AudioGeneratorWAV{
public:
    MusicClock()
    {   
     
    }

    void begin()
    {
        audioLogger = &Serial;
        file_ = new AudioFileSourcePROGMEM( azan_wav, sizeof(azan_wav) );
        out_ = new AudioOutputI2SNoDAC();
        AudioGeneratorWAV::begin(file_, out_);
    }
private:
    AudioFileSourcePROGMEM *file_;
    AudioOutputI2SNoDAC *out_;

};