#pragma once
#include <Arduino.h>



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

    Time():hour(0), minute(0), second(0)
    {

    }

    explicit Time(long timestamp)
    {
      this->hour = (timestamp / 3600) % 24;
      this->minute = (timestamp % 3600) / 60;
      this->second = timestamp % 60; 
    }

    explicit Time(int hour, int minute, int second)
    {
      this->hour = hour; 
      this->minute = minute;
      this->second = second;
    }

    Time addMinute(int minute)
    {
      int resMinute = minute + this->minute;  
      int hour_carry = resMinute / 60;
      resMinute = resMinute % 60; 
      int resHour = (this->hour + hour_carry) % 24; 
      return Time(resHour, resMinute, this->second);
    }

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

    void incrMinute()
    {
        this->minute += 1;
        this->hour += (this->minute >= 60 ? 1 : 0);
        this->minute = this->minute % 60;
        this->hour = this->hour % 24;
    }

    bool isMidNight()
    {
        return hour == 0 && minute == 0;
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