
#pragma once

#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "azan_clock.h"
#define STREAM_BUFFER_SIZE (1024)


class StreamAzan: public AudioGeneratorMP3{
public:
    void begin(const PRAYER& prayer)
    {
        audioLogger = &Serial;
        file_ = new AudioFileSourceICYStream();
        file_->RegisterMetadataCB(MDCallback, (void*)"ICY");
        file_->useHTTP10();
        file_->open(URL);
        buffer_ = new AudioFileSourceBuffer(file, STREAM_BUFFER_SIZE);
        buffer_->RegisterStatusCB(StatusCallback, (void*)"buffer");
        out_ = new AudioOutputI2SNoDAC();
        //don't play loud azan during Fajr
        if(prayer != Fajr)
            out_->SetGain(4.0);
        RegisterStatusCB(StatusCallback, (void*)"mp3");
        AudioGeneratorMP3::begin(buffer_, out_);
    }

    bool stop()
    {
       bool result = AudioGeneratorMP3::stop();
       // clear memory 
       delete file_; 
       delete buffer_; 
       delete out_;
       return result;
    }
    
        
protected:
    // Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
    static void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
    {
        const char *ptr = reinterpret_cast<const char *>(cbData);
        (void) isUnicode; // Punt this ball for now
        // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
        char s1[32], s2[64];
        strncpy_P(s1, type, sizeof(s1));
        s1[sizeof(s1)-1]=0;
        strncpy_P(s2, string, sizeof(s2));
        s2[sizeof(s2)-1]=0;
        Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
        Serial.flush();
    }

    // Called when there's a warning or error (like a buffer underflow or decode hiccup)
    static void StatusCallback(void *cbData, int code, const char *string)
    {
        const char *ptr = reinterpret_cast<const char *>(cbData);
        // Note that the string may be in PROGMEM, so copy it to RAM for printf
        char s1[64];
        strncpy_P(s1, string, sizeof(s1));
        s1[sizeof(s1)-1]=0;
        Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
        Serial.flush();
    }
private:
    AudioFileSourceICYStream *file_;
    AudioFileSourceBuffer *buffer_;
    AudioOutputI2SNoDAC *out_;
    const char *URL = "http://praytimes.org/audio/adhan/Sunni/Naghshbandi.mp3";
};