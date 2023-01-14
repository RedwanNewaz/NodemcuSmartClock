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
static const char* azanServerName  = "http://api.aladhan.com/v1/timingsByCity?city=NewOrleans&country=US&method=2";
static const float soundGain = 4.0;

static const char* hourTopic = "/clock/alarm/set/hour";
static const char* minuteTopic = "/clock/alarm/set/minute";
static const char* startTopic = "/clock/alarm/start";
static const char* resetTopic = "/clock/reset";


// start Sunday, March 13
static const Date daylight_start("13-03");
  // ends Sunday, November 6
static const Date daylight_ends("06-11");