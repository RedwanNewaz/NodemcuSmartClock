#include "smart_clock.h"

using namespace SMART_CLOCK; 

ntp_prayer::ntp_prayer()
{
    
}

String ntp_prayer::fetch_content(const char *url)
{
    WiFiClient client_;
    HTTPClient http_; 
    String payload_ = ""; 

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
    
        }
        http_.end();
    } else {
        Serial.printf("[AzanClock::HTTP] Unable to connect\n");
       
    }

    return payload_;
}


Time ntp_prayer::fetchTime()
{
    DynamicJsonDocument doc(1024);
    auto payload = fetch_content(azanServerName);
    deserializeJson(doc, payload);

    long timestamp = doc["data"]["date"]["timestamp"].as<long>();
    timestamp += timezone;

    Time curr; 
    curr.hour = (timestamp / 3600) % 24;
    curr.minute = (timestamp % 3600) / 60;
    curr.second = timestamp % 60; 

    int i = 0;
    for(auto prayer: PrayerNames)
    {
        String data = doc["data"]["timings"][prayer].as<String>();
        int hours = data.substring(0,2).toInt();
        int minutes = data.substring(3,5).toInt();
        Time time{hours, minutes, 0};
        prayers_[i++] = time;
    }
    return curr.offset();
}

void ntp_prayer::init()
{
    base_clock::init();
    mqtt_client::init();
}

void ntp_prayer::repeat()
{
    musicLoop();
    mqttLoop();
}


void ntp_prayer::notifyTime(const Time& curr, const Time& prayerTime, const String& prayerName)
{
    if(compareTime(curr, alarm_))
    {
        playSound(Alarm);
        setAlarm_ = false;
    }
    publishTime(curr, prayerTime, prayerName);
}