#pragma once

#include <Arduino.h>
#include "wifiCredential.h"

#define NUM_PRAYERS (5)

#define HOST_NAME "192.168.1.161"
#define USE_ARDUINO_OTA true

#define RESET_PIN   (D3)
#define MINIUTE_PIN (D1)
#define HOUR_PIN    (D2)
#define POWER_PIN   (D0)




struct Date{
  int month, day;
  
  Date(const String& date)
  {
    // convert string to int values
    day = date.substring(0, 2).toInt();
    month = date.substring(3, 5).toInt();
  }

  bool operator >= (const Date& other) const 
  {
    if(month > other.month)
      return true;
    else if(month == other.month && day >= other.day)
      return true;
    return false;
  }

  bool operator <= (const Date& other) const
  {
    if(month < other.month)
      return true;
    else if(month == other.month && day <= other.day)
      return true;
    return false;
  }
};



struct Time{
    int hour, minute, second;

    Time offset()
    {
         //button clock sometimes take long time to set up the clock 
        //we need to calculate offset for it including second as well 
        // delay for button: power (4) + memory (4) + minute (0.1) + hour (0.1) + finish (0.1)
        // clock offset is 40 sec
        second += 4 + 4  + 0.15 * minute  + 0.15 * hour + 0.15 + 40;

        // compute carry for minute and hour from currentSecond 
        int minute_carry = second / 60;
        minute += minute_carry;
        int hour_carry = minute / 60;
        hour += hour_carry;
        return *this;
    }

    bool operator == (const Time& other) const
    {
        return (this->hour == other.hour) && (this->minute == other.minute);
    }

    bool operator < (const Time& other) const 
    {
        auto currTimeInMin = hour * 60 + minute;
        auto otherTimeInMin = other.hour * 60 + other.minute;
        return currTimeInMin < otherTimeInMin; 
    }

      bool operator > (const Time& other) const 
    {
        auto currTimeInMin = hour * 60 + minute;
        auto otherTimeInMin = other.hour * 60 + other.minute;
        return currTimeInMin > otherTimeInMin; 
    }

    String paddedStr(int val) const 
    {
        String str = String(val);
        if(str.length() == 1)
            str = "0" + str;
        return str; 
    }

    String toString() const
    {
        String format = paddedStr(hour) + ":" + paddedStr(minute);
        return format;
    }
};

enum PrayerType{
    Fajr,
    Dhuhr,
    Asr,
    Maghrib,
    Isha
};

enum SoundType{
    Azan,
    Alarm
};

static const String PrayerNames[NUM_PRAYERS] = {"Fajr", "Dhuhr", "Asr", "Maghrib", "Isha"};
static const long timezone = -6; // GMT - 6
// static const char* net = "<your ssn>";
// static const char* password = "<your pass>";
static const char* azanServerName  = "http://api.aladhan.com/v1/timingsByCity?city=NewOrleans&country=US&method=2";
static const float soundGain = 4.0;

static const char* hourTopic = "/clock/alarm/set/hour";
static const char* minuteTopic = "/clock/alarm/set/minute";
static const char* startTopic = "/clock/alarm/start";


// start Sunday, March 13
static const Date daylight_start("13-03");
  // ends Sunday, November 6
static const Date daylight_ends("06-11");