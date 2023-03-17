#pragma once
#include <iostream>
#include <Arduino.h>
#include "config.h"
#include "../clock/ClockState.h"
#include "../clock/PrayerTime.h"
#include "ButtonInterface.h"

/**
  SMART CLOCK FEATURES
  (i)   internal clock update
  (ii)  automatic button control
  (iii) prayer time alert
  (iv)  preset alarm (e.g., washing, drayer, pizza)
  (v)   sleep time alarm (wake up in the morning)
  (vi)  mqtt inteface to communicate with cell phone
**/

class ClockInterface :public ButtonInterface::Clock{
public:
  ClockInterface(){
     bool azanActive = false; 
    _nextPrayer = nextPrayerIndex(azanActive);
  }

  ClockInterface(const ClockTime &time) : _currentTime(time) {
    updatePrayers();
     bool azanActive = false; 
    _nextPrayer = nextPrayerIndex(azanActive);
    _currentTime = _currentTime.offset();
    setTime(_currentTime.getHour(), _currentTime.getMinute());
  }



  void reset(const ClockTime &time) {
    _currentTime = time;
    Serial.printf("[+] resetting time = %02d:%02d \n", _currentTime.getHour(), _currentTime.getMinute());
    updatePrayers();
     bool azanActive = false; 
    _nextPrayer = nextPrayerIndex(azanActive);
    _currentTime = _currentTime.offset();
    setTime(_currentTime.getHour(), _currentTime.getMinute());
  }

  //----------------virtual functions----------------------------
  virtual void playSound(const SoundType& sound) = 0;
  virtual void publishTime(const ClockTime& time, const ClockTime& prayerTime, const ClockTime& alarmTime, const char* prayerName) = 0;

  //--------------------------------------------------------------

  bool init_sound()
  {
    playSound(Boot);
    _stateMachine.setMusic(true);
    // don't repeat this timer 
    return false;
  }

  bool update_every_minute(void *param) {
    // update internal timer
    _currentTime.incrMinute();
    //Serial.printf("[+] current time = %02d:%02d \n", _currentTime.getHour(), _currentTime.getMinute());

 
    bool azanActive = false; 
    _nextPrayer = nextPrayerIndex(azanActive);
    // update state machine 
    _stateMachine.setAzan(azanActive);
    if (_stateMachine.playAzan()) {
      // activate azan sound
      Serial.printf("playing azan sound \n");
      playSound(Azan);

    } else if (_stateMachine.playAlarm()) {
      // activate alarm sound
      Serial.printf("playing alarm sound \n");
      playSound(Alarm);
      // reset alarm time
      _alarmTime = ClockTime();
    }
    
    // is it a New day? then update prayer times
    if (_currentTime.isMidNight())
      updatePrayers();

    return true; 
  }

  bool update_frequently(void *param)
  {
    // check activities for music clock
    String prayerNames[NUM_PRAYERS] = {"Fajr", "Dhuhr", "Asr", "Maghrib", "Isha"};

        // Serial.println("");
    publishTime(_currentTime, _prayerTimes[_nextPrayer], _alarmTime, prayerNames[_nextPrayer].c_str());
           Serial.printf("[Next Prayer] %s = %02d:%02d \n", prayerNames[_nextPrayer], _prayerTimes[_nextPrayer].getHour(),
       _prayerTimes[_nextPrayer].getMinute());

    return true; 
  }

  void setAlarmTime(const ClockTime &alarm) {
    _alarmTime = alarm;
    _stateMachine.setAlarm(true);
    // publish alarm time 
    update_frequently(nullptr);
  }

  void cancelAlarmTime() {
    _alarmTime = ClockTime();
    _stateMachine.setAlarm(false);
    // publish alarm time 
    update_frequently(nullptr);
  }

protected:
  ClockTime _currentTime;
  ClockTime _alarmTime;
  ClockState _stateMachine;
  ClockTime _prayerTimes[NUM_PRAYERS];
  PrayerTime _prayers;
  int _nextPrayer;

private:
  void updatePrayers() {
    _prayers.updateDate(_currentTime.getDate());
    _prayers.get(_prayerTimes);
  }

  int nextPrayerIndex(bool& azanActive)
  {
    int nextPrayer = 0;
    
    for (int i = 0; i < NUM_PRAYERS; ++i) 
    {      
      if(_currentTime > _prayerTimes[i])
        nextPrayer = (i + 1) % NUM_PRAYERS;
        azanActive = (azanActive || (_currentTime == _prayerTimes[i]));
    }
    return nextPrayer;
  }
};
