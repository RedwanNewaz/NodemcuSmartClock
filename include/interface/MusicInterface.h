#pragma once 
#include "ClockInterface.h"
#include "../data/azan_sound.hpp"
#include "../data/bell_sound.hpp"

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

class MusicInterface: public ClockInterface, public AudioGeneratorWAV{
public:
    MusicInterface()
    {   
        play_ = false;
    }

    MusicInterface(const ClockTime &time):ClockInterface(time)
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
        file_ = (sound == Azan) ? new AudioFileSourcePROGMEM( azan_wav, sizeof(azan_wav) ) : new AudioFileSourcePROGMEM( bell_wav, sizeof(bell_wav) );
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

    bool tick()
    {

        if (play_ && AudioGeneratorWAV::isRunning()) {
            if (!AudioGeneratorWAV::loop())
            {
                Serial.println("[MusicPlayer]: sound finishes" );
                AudioGeneratorWAV::stop();  
                play_ = false;
                _stateMachine.setMusic(false);
                file_->close();
                delete file_;
                delete out_;
            }
        }
        return play_;
    }

private:
    AudioFileSourcePROGMEM *file_;
    AudioOutputI2SNoDAC *out_;
    bool play_;

};