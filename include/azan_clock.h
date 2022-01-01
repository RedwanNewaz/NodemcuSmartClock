/* created by Redwan Newaz 
* nodemcu cannot compute prayer times accurately using the PrayerTimes library
* therefore we need to invoke the prayer times from the server bellow 
* static const char * url = "http://api.aladhan.com/v1/calendar?latitude=36.1092602&longitude=-79.8216712&method=2&month=12&year=2021";
* this server will reponse with a json file which needs to be decoded for computing prayer time    
*/
#pragma once 
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "wifi_manager.h"

extern WifiManager::Manager manager; 

#define NUM_DAILY_PRAYERS (5)

class AzanClock{
public:
    AzanClock()
    {

    }

    void decode_html_string(const String& html)
    {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, html);
        
        int avoid_indexes[2] = {1 , 4}; // these are sunrise and sunset times  
        int count  = 0;
        for (int i = 0; i < 7; i++)
        {
            String data = doc["data"][0]["timings"][prayers[i]].as<String>();
            Serial.print(prayers[i]); Serial.print("\t\t:\t");
            Serial.println(data);

            if(i == avoid_indexes[0] || i == avoid_indexes[1])
                continue;

            // hard coded numeric conversion 14:55
            // FIXME: what if server is down?
            // TODO we need to create a timer task for repeat this again 
            int hours = data.substring(0,2).toInt();
            int minutes = data.substring(3,5).toInt();
            daily_prayer_times_[count++] = hours * 60 + minutes; 
        } 
    }

    void fetch_content(const char *url)
    {
        WiFiClient client_;
        HTTPClient http_;    
  
        Serial.print("[AzanClock::HTTP] begin...\n");
        if (http_.begin(client_, url)) 
        {  // HTTP
            Serial.print("[AzanClock::HTTP] GET...\n");
            // start connection and send HTTP header
            int httpCode = http_.GET();
            // httpCode will be negative on error
            if (httpCode > 0) {
                decode_html_string(http_.getString());
            } else {
                Serial.printf("[AzanClock::HTTP] GET... failed, error: %s\n", http_.errorToString(httpCode).c_str());
            }
            http_.end();
        } else {
            Serial.printf("[AzanClock::HTTP] Unable to connect\n");
        }
    }

    void update_clock(int year, int month, int day)
    {
        // TODO store method ISNA to eeprom 
        // manager is a wifi manager instance which manages eeprom data  
        auto loc = manager.get_location();

        String latitude         = loc.latitude;
        String longitude        = loc.longitude;
        String my_latitude      = "http://api.aladhan.com/v1/calendar?latitude=" + latitude; 
        String my_longitude     = "&longitude=" + longitude; 
        String my_calc_method   = "&method=2"; // ISNA
        String my_month         = "&month=" + String(month);
        String my_year          = "&year=" + String(year);
        String timeline_url = my_latitude + my_longitude + my_calc_method + my_month + my_year;

        Serial.printf("[AzanClock::HTTP] url = %s \n",timeline_url.c_str());
        fetch_content(timeline_url.c_str()); 

        // debug whether conversion is appropriate 
        Serial.println("\n\n[AzanClock::HTTP] minute values for 5 prayers \n");
        for (size_t i = 0; i < NUM_DAILY_PRAYERS; i++)
        {
            Serial.print(prayers[i]); Serial.print("\t\t:\t");
            Serial.println(daily_prayer_times_[i]);
        }
            
    }

    int next_prayer_in_minutes(int currentTimeInMin)
    {

        for (int i = 0; i < NUM_DAILY_PRAYERS; i++)
        {
            if(daily_prayer_times_[i] > currentTimeInMin)
            {
                Serial.print("[AzanClock] Next prayer is ");
                Serial.println(daily_prayer_names_[i]);
                return daily_prayer_times_[i] - currentTimeInMin; 
            }
        }
        // this happens after isha only 
        // after isha we need to wait until 12:00 AM to count time for the next day 
        Serial.print("[AzanClock] Next prayer is ");
        Serial.println(daily_prayer_names_[0]);
        return daily_prayer_times_[0] - currentTimeInMin + 24 * 60;
        

    }

private:
    int daily_prayer_times_[NUM_DAILY_PRAYERS];
    String daily_prayer_names_[NUM_DAILY_PRAYERS];
    const String prayers[7] = {"Fajr", "Sunrise", "Dhuhr", "Asr", "Sunset", "Maghrib", "Isha"};
    const char* latitude;
    const char* longitude;
};