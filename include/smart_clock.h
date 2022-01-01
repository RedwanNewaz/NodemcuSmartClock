#pragma once 
#include <Arduino.h>
#include <NTPClient.h>
#include "button_clock.h"
#include "azan_clock.h"
#include "music_clock.h"

class SmartClock: public NTPClient{
public:
    SmartClock(UDP& udp, const char* poolServerName, ButtonClock::Clock& bcc, AzanClock& azan):
    NTPClient(udp, poolServerName), bcc_(bcc), azan_(azan)
    {

    }

    void reset_clock()
    {
        Serial.println(" \n \t\t [SmartClock] Updating clock ...");
        update();
        String formattedTime = getFormattedTime();
        Serial.print("[SmartClock] Formatted Time: ");
        Serial.println(formattedTime);  

        currentHour_ = getHours();
        currentMinute_ = getMinutes();
        currentSecond_ = getSeconds();
        bcc_.set_time(currentHour_, currentMinute_);

        // get day, month and year
        for (size_t i = 0; i < 2; i++)
        {
            // sometimes it does not get it right at first try
            time_t rawtime = this->getEpochTime();
            currentDay_ = getDate(rawtime);
            currentMonth_ = getMonth(rawtime);
            currentYear_ = getYear(rawtime);
        }
        
       

        Serial.print("*******  [SmartClock] Calander date : ");
        Serial.print(currentMonth_);  Serial.print("/");Serial.print(currentDay_);  Serial.print("/");Serial.print(currentYear_);  Serial.print(" *******\n\n");


        // update azan clock 
        azan_.update_clock(currentYear_, currentMonth_, currentDay_);
        prayerAlarm_ = azan_.next_prayer_in_minutes(getCurrentTimeInMinutes());
        Serial.print("[SmartClock] next prayer coming in ");
        Serial.print(prayerAlarm_);
        Serial.println(" minutes");

    }

    void update_clock()
    {
        // this will perform software update 
        ++currentMinute_;
        --prayerAlarm_;
        if(currentMinute_ >= 60)
        {
            currentHour_ = (currentHour_ + 1) % 24;
            currentMinute_ = 0;
        }

        // check prayer alarm when prayerAlarm is up
        if(prayerAlarm_ == 0)
        {
            Serial.println("[SmartClock] AZAN time, go to pray ...");
            prayerAlarm_ = azan_.next_prayer_in_minutes(getCurrentTimeInMinutes());
        }
        else
        {
            Serial.print("[SmartClock] next prayer is coming in ");
            Serial.print(prayerAlarm_);
            Serial.println(" minutes");
        }

        // every day sync clock at 12:01 AM
        if(currentHour_ == 0 && currentMinute_ == 1)
        {
            Serial.println("[SmartClock] synchronizing with the ntp and azan clocks");
            reset_clock();
        } 
    }

    int next_prayer()
    {
        return prayerAlarm_;
    }
private:
    ButtonClock::Clock bcc_;
    AzanClock azan_;
    volatile int currentHour_, currentMinute_, currentSecond_; 
    volatile int currentDay_, currentMonth_, currentYear_; 
    volatile int prayerAlarm_; 



protected:
    int getYear(time_t& rawtime) 
    {
        
        struct tm * ti;
        ti = localtime (&rawtime);
        int year = ti->tm_year + 1900;        
        return year;
    }

    int getMonth(time_t& rawtime) 
    {
        struct tm * ti;
        ti = localtime (&rawtime);
        int month = (ti->tm_mon + 1) < 10 ? 0 + (ti->tm_mon + 1) : (ti->tm_mon + 1);
        
        return month;
    }

    int getDate(time_t& rawtime) 
    {
        struct tm * ti;
        ti = localtime (&rawtime);
        int day = (ti->tm_mday) < 10 ? 0 + (ti->tm_mday) : (ti->tm_mday);        
        return day;
    }

    int getCurrentTimeInMinutes()
    {
        return currentHour_ * 60 + currentMinute_;
    }

};