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
    // debounce buttons for setting the current time
    setTime(current_.hour, current_.minute);
    
    // change initialized flag so that update can be done
    this->initialized_ = true;
    // play sound as a confirmation of initialization
    // delay(1000);
    // playSound(Alarm);
}

void base_clock::updateTimer() {
    // sanity check
    if (!initialized_){
        init();
        return;
    }
    // increment current time a minute
    current_.minute += 1;
    current_.hour += (current_.minute >= 60 ? 1 : 0);
    current_.minute = current_.minute % 60;
    current_.hour = current_.hour % 24;

    // compare current time with prayer time to play sound
    bool playingAzan = false;
    int lastPrayerIndex = 0;
    for (int i = 0; i < NUM_PRAYERS; ++i) {
        if(compareTime(current_, prayers_[i]))
        {
            playSound(Azan);
            playingAzan = true;
        }

        if( current_ > prayers_[i])
            lastPrayerIndex = i;
    }

    int nextPrayerIndex = (current_ < prayers_[0]) ? 0 : ++lastPrayerIndex % NUM_PRAYERS;
    Time nextPrayerTime = prayers_[nextPrayerIndex];
    String nextPrayer = PrayerNames[nextPrayerIndex];
    // reset clock at 24:00
    initialized_ = (current_.hour + current_.minute) != 0;
    if(!playingAzan)
        notifyTime(current_, nextPrayerTime, nextPrayer);

}

bool base_clock::compareTime(const Time &A, const Time &B) {
    return A == B;
}
