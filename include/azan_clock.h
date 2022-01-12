/* created by Redwan Newaz 
* nodemcu 0.9 cannot compute prayer times accurately using the PrayerTimes library
* https://github.com/asmaklad/Arduino-Prayer-Times
* therefore we need to invoke the prayer times from the server bellow 
* static const char * url = "http://api.aladhan.com/v1/calendar?latitude=xxx&longitude=yyy&method=2&month=12&year=2021";
* this server will reponse with a json file which needs to be decoded for computing prayer time    
*/
#pragma once 
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <RemoteDebug.h>
#include <WiFiClient.h>
#include "wifi_manager.h"

extern WifiManager::Manager manager; 
extern RemoteDebug Debug;


#define NUM_DAILY_PRAYERS (5)

class AzanClock{
public:
    /**
     * @brief Construct a new Azan Clock object
     * Nodemcu has a limited computational resources. \par 
     * Existing prayer time library cannot compute prayer time accurately for nodemcu but it works perfect in a desktop environment. \par 
     * Threfore, we rely on aladhan website api to compute prayer times
     */
    AzanClock()
    {

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
                // decode_html_string(http_.getString(), day);
                payload_ = http_.getString();
            } else {
                Serial.printf("[AzanClock::HTTP] GET... failed, error: %s\n", http_.errorToString(httpCode).c_str());
                debugE("[AzanClock::HTTP] GET... failed, error: %s\n", http_.errorToString(httpCode).c_str());

            }
            http_.end();
        } else {
            Serial.printf("[AzanClock::HTTP] Unable to connect\n");
            debugE("[AzanClock::HTTP] Unable to connect");
        }
    }


    /**
     * @brief azan server sends a big chunk of data in string format \par 
     * we need to decode this data to compute the prayer time \par 
     * Luckily this data is in json format and I am hoping they maintain same standard all the time. \par 
     * While decoding time from string format requires so many complex operations --split, string to int conversion,
     * we can simply hard code the postion of hour and minutes to avoid such complex operation for nodemcu. 
     * 
     * @param html file in string format 
     */
    void update_clock()
    {
        // get prayer method from the eeprom 
        DynamicJsonDocument doc(1024);
        
        String url = "http://ipinfo.io/json";
        fetch_content(url.c_str()); 
        deserializeJson(doc, payload_);
        String country = doc["country"].as<String>();
        String city = doc["city"].as<String>();
        timezone_ = doc["timezone"].as<String>();

        // manager is a wifi manager instance which manages eeprom data  
        String timeline_url = "http://api.aladhan.com/v1/timingsByCity?city=" + city + "&country=" + country + "&method=2";

        Serial.printf("[AzanClock::HTTP] url = %s \n",timeline_url.c_str());
        fetch_content(timeline_url.c_str());
      
        
        deserializeJson(doc, payload_);
        timestamp_ = doc["date"]["timestamp"].as<long>();
        
        int avoid_indexes[2] = {1 , 4}; // these are sunrise and sunset times  
        int count  = 0;
        for (int i = 0; i < 7; i++)
        {
            String data = doc["data"]["timings"][prayers[i]].as<String>();
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

    int next_prayer_in_minutes(int currentTimeInMin)
    {

        for (int i = 0; i < NUM_DAILY_PRAYERS; i++)
        {
            if(daily_prayer_times_[i] > currentTimeInMin)
            {
                Serial.print("[AzanClock] Next prayer is ");
                Serial.println(daily_prayer_names_[i]);
                debugI("[AzanClock] Next prayer is %s", daily_prayer_names_[i].c_str());
                return daily_prayer_times_[i] - currentTimeInMin; 
            }
        }
        // this happens after isha only 
        // after isha we need to wait until 12:00 AM to count time for the next day 
        Serial.print("[AzanClock] Next prayer is ");
        Serial.println(daily_prayer_names_[0]);
        debugI("[AzanClock] Next prayer is Fajr");
        return daily_prayer_times_[0] - currentTimeInMin + 24 * 60;
        

    }
    const char *get_timezone()
    {
        return timezone_.c_str();
    }

    long get_timestamp()
    {
        return timestamp_;
    }

private:
    int daily_prayer_times_[NUM_DAILY_PRAYERS];
    String daily_prayer_names_[NUM_DAILY_PRAYERS];
    const String prayers[7] = {"Fajr", "Sunrise", "Dhuhr", "Asr", "Sunset", "Maghrib", "Isha"};
    String payload_; 
    String timezone_;
    long timestamp_;

};