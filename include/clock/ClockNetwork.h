#pragma once
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class ClockNetwork: public NTPClient
{
public:
  ClockNetwork(UDP& udp, const char* poolServerName):NTPClient(udp, poolServerName)
  {
    begin();
    // setTimeOffset(3600 * -5);
  }

  String fetchTime()
  {
    update();
    unsigned long epochTime = getEpochTime();
    //Get a time structure
    struct tm *ptm = gmtime ((time_t *)&epochTime); 

    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon+1;
    int currentYear = ptm->tm_year+1900;
    String currentDate = paddedStr(monthDay) + "." + paddedStr(currentMonth) + "." + String(currentYear);
    //HH:MM:SS
    // int currentHour = getHours();
    // int currentMinute = getMinutes();
    // String formattedTime = currentDate + " " + paddedStr(currentHour) + ":" + paddedStr(currentMinute);

    String formattedTime = currentDate + " " + getFormattedTime();
    
    return formattedTime;
  }
private:
  String paddedStr(int val) const 
  {
      String str = String(val);
      if(str.length() == 1)
          str = "0" + str;
      return str; 
  }

};