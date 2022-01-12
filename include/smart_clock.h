#pragma once 
#include <Arduino.h>
#include <NTPClient.h>
#include <RemoteDebug.h>
#include "button_clock.h"
#include "azan_clock.h"
#include "music_clock.h"
#define PRECISE_CLOCK

extern RemoteDebug Debug;
extern MusicClock wav;

class SmartClock: public NTPClient{
public:
    SmartClock(UDP& udp, const char* poolServerName, ButtonClock::Clock& bcc, AzanClock& azan):
    NTPClient(udp, poolServerName), bcc_(bcc), azan_(azan)
    {

    }

    /**
     * @brief This function will synchronize time with ntp server \par
     * Then it will invoke the button clock to change the clock display time \par
     * Finally it will synchronize the azan time for particular date
     */
    void reset_clock()
    {
        // update azan clock for getting time zone 
        // azan_.update_clock();
        // setTZ(azan_.get_timezone());

        Serial.println(" \n \t\t [SmartClock] Updating clock ...");
        update();
        String formattedTime = getFormattedTime();
        Serial.print("[SmartClock] Formatted Time: ");
        Serial.println(formattedTime);  

        currentHour_ = getHours();
        currentMinute_ = getMinutes();
        currentSecond_ = getSeconds();

#ifdef PRECISE_CLOCK
        //button clock sometimes take long time to set up the clock 
        //we need to calculate offset for it including second as well 
        // delay for button: power (4) + memory (4) + minute (0.1) + hour (0.1) + finish (0.1)
        currentSecond_ += 4 + 4  + 0.15 * currentMinute_  + 0.15 * currentHour_ + 0.15;

        // compute carry for minute and hour from currentSecond 
        int minute_carry = currentSecond_ / 60;
        currentMinute_ += minute_carry;
        int hour_carry = currentMinute_ / 60;
        currentHour_ += hour_carry;

        // second and minute cannot be more than 60
        currentSecond_ = currentSecond_ % 60;
        currentMinute_ = currentMinute_ % 60;
        // hour cannot be more than 24
        currentHour_ = currentHour_ % 24;
#endif  


        bcc_.set_time(currentHour_, currentMinute_);

        azan_.update_clock();

        // get day, month and year
        for (size_t i = 0; i < 2; i++)
        {
            // sometimes it does not get it right at first try
            time_t rawtime = azan_.get_timestamp();
            currentDay_ = getDate(rawtime);
            currentMonth_ = getMonth(rawtime);
            currentYear_ = getYear(rawtime);
        }
          

        Serial.print("*******  [SmartClock] Calander date : ");
        Serial.print(currentMonth_);  Serial.print("/");Serial.print(currentDay_);  Serial.print("/");Serial.print(currentYear_);  Serial.print(" *******\n\n");
        // azan_.update_clock();
        prayerAlarm_ = azan_.next_prayer_in_minutes(getCurrentTimeInMinutes());
        Serial.print("[SmartClock] next prayer coming in ");
        Serial.print(prayerAlarm_);
        Serial.println(" minutes");

    }

    /**
     * @brief In order to compute next prayer time we need to periodically update internal smart clock variables. \par 
     * Smart clock clock gets updated every minute from the timer function (defined in main.cpp) \par
     * The timer function maintains a priority list where update_clock() function appears first and then repeat_azan_clock (defined in main.cpp) \par 
     * This way repeat_azan_clock function skips current azan time whose wav sound will be performing in the next iteration in void loop() function \par 
     * and sets the timer for the next prayer time
     */

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
            debugI("[SmartClock] AZAN time, go to pray ...");
            wav.begin();
            prayerAlarm_ = azan_.next_prayer_in_minutes(getCurrentTimeInMinutes());
        }
        else
        {
            Serial.print("[SmartClock] next prayer is coming in ");
            Serial.print(prayerAlarm_);
            Serial.println(" minutes");
            debugI("[SmartClock] next prayer coming in %d minutes", prayerAlarm_);
        }

        // every day sync clock at 12:01 AM
        if(currentHour_ == 0 && currentMinute_ == 1)
        {
            Serial.println("[SmartClock] synchronizing with the ntp and azan clocks");
            debugW("[SmartClock] synchronizing with the ntp and azan clocks");
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