#pragma once 

#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2SNoDAC.h>
#include "azan_sound_short.h"
#include "bell_sound.h"
#include "base_clock.h"
class MusicClock: public base_clock, public AudioGeneratorWAV{
public:
    MusicClock()
    {   
        play_ = false;
    }

    /**
     * @brief azan wav file get deleted after each run \par
     * Therefore, this function needs to call every prayer time \par 
     * TODO we can play multiple azans if it fits in the rom 
     */
    void begin(const SoundType& sound)
    {        
        audioLogger = &Serial;
        // file_ = (sound == Azan) ? new AudioFileSourcePROGMEM( azan_wav, sizeof(azan_wav) ) : new AudioFileSourcePROGMEM( bell_wav, sizeof(bell_wav) );
        if (sound == Azan)
            file_ = new AudioFileSourcePROGMEM( azan_wav, sizeof(azan_wav) ) ;
        else
            file_ = new AudioFileSourcePROGMEM( bell_wav, sizeof(bell_wav) ) ;
        out_ = new AudioOutputI2SNoDAC();
        out_->SetGain(soundGain);
        AudioGeneratorWAV::begin(file_, out_);
        Serial.println("[MusicClock] Azan is loaded to play");
    }

    void playSound(const SoundType& sound)
    {
        play_ = true;
        begin(sound);
    }

    void musicLoop()
    {
        if(!play_) return;

        if (AudioGeneratorWAV::isRunning()) {
            if (!AudioGeneratorWAV::loop())
            {
                AudioGeneratorWAV::stop();  
                play_ = false;
            }
        }
    }

private:
    AudioFileSourcePROGMEM *file_;
    AudioOutputI2SNoDAC *out_;
    bool play_;

};