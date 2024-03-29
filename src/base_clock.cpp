//
// Created by Abdullah Al Redwan Newaz on 7/31/22.
//

#include "base_clock.h"

base_clock::base_clock() {
    this->initialized_ = false;

}

void base_clock::init() {
    // fetch current time 
    // and prayer times from internet
    // updatePrayerTime(prayers_);
    current_ = fetchTime();

    delay(1000);
    // debounce buttons for setting the current time
    setTime(current_.hour, current_.minute);
    
    // change initialized flag so that update can be done
    this->initialized_ = true;
    // play sound as a confirmation of initialization
    // delay(1000);
    // playSound(Alarm);
    playingAzan_ = false;
}

void base_clock::updateClock()
{

   // sanity check
    if (!initialized_){
        init();
        return;
    }
    // increment current time a minute
    current_.incrMinute();

    // reset clock at 24:00
    initialized_ = !current_.isMidNight();
    playingAzan_ = false;
}


void base_clock::updateTimer() {
 
    if(!initialized_)
        return;

    // compare current time with prayer time to play sound
    
    int lastPrayerIndex = 0;
    for (int i = 0; i < NUM_PRAYERS; ++i) {
        if(compareTime(current_, prayers_[i]) && !playingAzan_)
        {
            playSound(Azan);
            playingAzan_ = true;
        }

        if( current_ > prayers_[i])
            lastPrayerIndex = i;
    }

    int nextPrayerIndex = (current_ < prayers_[0]) ? 0 : ++lastPrayerIndex % NUM_PRAYERS;
    Time nextPrayerTime = prayers_[nextPrayerIndex];
    String nextPrayer = PrayerNames[nextPrayerIndex];
  
    notifyTime(current_, nextPrayerTime, nextPrayer);

}

bool base_clock::compareTime(const Time &A, const Time &B) {
    return A == B;
}
