
#pragma once

#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"



class StreamAzan: public AudioGeneratorMP3{
public:
    void begin()
    {
        audioLogger = &Serial;
        file = new AudioFileSourceICYStream();
        file->RegisterMetadataCB(MDCallback, (void*)"ICY");
        file->useHTTP10();
        file->open(URL);
        auto buff = new AudioFileSourceBuffer(file, 2048);
        buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
        out = new AudioOutputI2SNoDAC();
        RegisterStatusCB(StatusCallback, (void*)"mp3");
        AudioGeneratorMP3::begin(buff, out);
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
    // AudioGeneratorMP3 *mp3;
    AudioFileSourceICYStream *file;
    // AudioFileSourceBuffer *buff;
    AudioOutputI2SNoDAC *out;
    const char *URL = "http://praytimes.org/audio/adhan/Sunni/Naghshbandi.mp3";
};