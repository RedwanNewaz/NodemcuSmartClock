//
// Created by Abdullah Al Redwan Newaz on 7/31/22.
//

#ifndef HELLOWORLD_BASE_CLOCK_H
#define HELLOWORLD_BASE_CLOCK_H
#include "config.h"

class base_clock {
public:
    base_clock();
    /**
     * call this function one time to set clock hour and minutes automatically
     * It will fetch time from NTP server to setup clock time
     */
    void init();
    /**
     * call this updateTimer function every minute to update internal clock
     * This will play sound for prayer time
     */
    void updateTimer();
    void updateClock();

    virtual void musicLoop() = 0;
    
protected:
    virtual void setTime(int hour, int minute) = 0;
    virtual void playSound(const SoundType& sound) = 0;
    virtual Time fetchTime() = 0;
    virtual void notifyTime(const Time& curr, const Time& prayerTime, const String& prayerName) = 0;

    bool compareTime(const Time& A, const Time& B);
    Time prayers_[NUM_PRAYERS];
    Time current_;
    bool playingAzan_;
private:
    bool initialized_;
    

};


#endif //HELLOWORLD_BASE_CLOCK_H
