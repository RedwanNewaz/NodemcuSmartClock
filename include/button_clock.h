#pragma once 

#include <Arduino.h>


namespace ButtonClock{
  class Button{
    public:
    Button(const String name, int pin, int cycle, int delay, bool reversed):
    pin_(pin), cycle_(cycle), delay_(delay), name_(name), reversed_(reversed)
    {
        // the button is reversed when it goes from high state to low state, e.g., power button 

    }
    void execute()
    {
      int state = 1 - reversed_;
      for(int i = 0; i < cycle_; ++i)
      {
        // forward 
        digitalWrite(pin_, state);
        delay(delay_);
        // reverse 
        digitalWrite(pin_, 1 - state);
        delay(delay_);
      }
    }
    private:
      int pin_, cycle_, delay_;
      bool reversed_;
      String name_;
  };

  class Clock{
    public:
    Clock(int power_pin, int reset_pin, int hour_pin, int minute_pin):
    power_pin_(power_pin), reset_pin_(reset_pin), hour_pin_(hour_pin), minute_pin_(minute_pin)
    {
        this->reset_time_ = 2000;
        this->debounce_time_ = 50; 
        pinMode(hour_pin, OUTPUT);
        pinMode(minute_pin, OUTPUT);
        pinMode(reset_pin, OUTPUT);
        pinMode(power_pin, OUTPUT);

        digitalWrite(hour_pin, LOW);
        digitalWrite(minute_pin, LOW);
        digitalWrite(reset_pin, LOW);
        digitalWrite(power_pin, HIGH);
    }

    void set_time(int num_hour, int num_minute)
    {
      // cycle power 
      Button power("power", power_pin_, 1, reset_time_, true);
      Button memory("memory", reset_pin_, 1, reset_time_, false);
      Button hour("hour", hour_pin_, num_hour, debounce_time_, false);
      Button minute("minute", minute_pin_, num_minute, debounce_time_, false);
      Button finish("finish", reset_pin_, 1, debounce_time_, false);
      
      // execute buttons 
      power.execute();
      memory.execute();
      hour.execute();
      minute.execute();
      finish.execute();

      
    }

    private:
      int power_pin_, reset_pin_, hour_pin_, minute_pin_;
      int reset_time_, debounce_time_;
    

  };

  

}