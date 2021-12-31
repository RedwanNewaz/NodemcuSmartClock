#pragma once 
#include <Arduino.h>
#include <NTPClient.h>
#include "button_clock.h"

class SmartClock: public NTPClient{
public:
    SmartClock(UDP& udp, const char* poolServerName, ButtonClock::Clock& bcc):
    NTPClient(udp, poolServerName), bcc_(bcc)
    {

    }

    void reset_clock()
    {
        Serial.println(" \n \t\t Updating clock ...");
        update();
        String formattedTime = getFormattedTime();
        Serial.print("Formatted Time: ");
        Serial.println(formattedTime);  

        currentHour_ = getHours();
        currentMinute_ = getMinutes();
        currentSecond_ = getSeconds();
        bcc_.set_time(currentHour_, currentMinute_);

    }

    void update_clock()
    {
        // this will perform software update 
        ++currentMinute_;
        update();
        String formattedTime = getFormattedTime();
        Serial.print("Formatted Time: ");
        Serial.println(formattedTime); 
        Serial.print("Current Minute: ");
        Serial.println(currentMinute_);

        if(currentMinute_ >= 60)
        {
            ++currentHour_;
            currentMinute_ = 0;
        }
    }
private:
    ButtonClock::Clock bcc_;
    int currentHour_, currentMinute_, currentSecond_; 

};