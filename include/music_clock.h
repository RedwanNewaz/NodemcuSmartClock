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

    /**
     * @brief azan wav file get deleted after each run \par
     * Therefore, this function needs to call every prayer time \par 
     * TODO we can play multiple azans if it fits in the rom 
     */
    void begin()
    {        
        audioLogger = &Serial;
        file_ = new AudioFileSourcePROGMEM( azan_wav, sizeof(azan_wav) );
        out_ = new AudioOutputI2SNoDAC();
        out_->SetGain(4.0);
        AudioGeneratorWAV::begin(file_, out_);
        Serial.println("[MusicClock] Azan is loaded to play");
    }

    void forceStop()
    {
        running = false; 
    }
private:
    AudioFileSourcePROGMEM *file_;
    AudioOutputI2SNoDAC *out_;

};