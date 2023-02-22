#pragma once

#include <Arduino.h>
#include "wifiCredential.h"
#include "date_time.h"

#define NUM_PRAYERS (5)

#define HOST_NAME "192.168.1.161"
#define USE_ARDUINO_OTA true

#define RESET_PIN   (D3)
#define MINIUTE_PIN (D1)
#define HOUR_PIN    (D2)
#define POWER_PIN   (D0)



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
// "http://api.aladhan.com/v1/timingsByCity?city=New%20Orleans&country=US&method=2"
static const char* azanServerName  = "http://api.aladhan.com/v1/timingsByCity?city=New%20Orleans&country=US&method=2";
static const float soundGain = 4.0;

// subscribe topic 
static const char* hourTopic = "/clock/alarm/set/hour";
static const char* minuteTopic = "/clock/alarm/set/minute";
static const char* startTopic = "/clock/alarm/start";
static const char* resetTopic = "/clock/reset";
static const char* presetTopic = "/clock/alarm/preset";
static const char* syncTopic = "/clock/time/sync";

//publish topic 
static const char* pubTime = "/clock/time";
static const char* pubAlarm = "/clock/alarm";
static const char* pubPrayerTime = "/clock/prayer/time";
static const char* pubPrayerName = "/clock/prayer/name";


// start Sunday, March 13
static const Date daylight_start("13-03");
  // ends Sunday, November 6
static const Date daylight_ends("06-11");