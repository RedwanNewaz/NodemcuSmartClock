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

    /* @brief this function needs to be invoked every time when we play azan \par
    * azan wav file get deleted after each run
    * TODO we can play multiple azans if it fits in the rom 
    */
    void begin()
    {        
        audioLogger = &Serial;
        file_ = new AudioFileSourcePROGMEM( azan_wav, sizeof(azan_wav) );
        out_ = new AudioOutputI2SNoDAC();
        AudioGeneratorWAV::begin(file_, out_);
        Serial.println("[MusicClock] Azan is loaded to play");
    }
private:
    AudioFileSourcePROGMEM *file_;
    AudioOutputI2SNoDAC *out_;

};