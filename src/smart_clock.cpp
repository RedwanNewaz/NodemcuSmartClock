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

    
    // add daylight saving automatically 
    String dateStr = doc["data"]["date"]["gregorian"]["date"].as<String>();
    Date date(dateStr);
    int offset = 0;
    if(date >= daylight_start && date <= daylight_ends)
        offset = 1;
    

    long timestamp = doc["data"]["date"]["timestamp"].as<long>();
    timestamp += (timezone + offset) * 3600;

    Time curr(timestamp); 
 

    int i = 0;
    for(auto prayer: PrayerNames)
    {
        String data = doc["data"]["timings"][prayer].as<String>();
        int hours = data.substring(0,2).toInt();
        int minutes = data.substring(3,5).toInt();
        Time time(hours, minutes, 0);
        prayers_[i++] = time;
    }
    return curr.offset();
}

void ntp_prayer::init()
{
    base_clock::init();
    mqtt_client::init();
}

void ntp_prayer::reset(int type)
{
    if(type == 1)
        setTime(current_.hour, current_.minute);
    else
        base_clock::init();
}

void ntp_prayer::repeat()
{
    musicLoop();
    mqttLoop();
}


void ntp_prayer::notifyTime(const Time& curr, const Time& prayerTime, const String& prayerName)
{
    if(setAlarm_ && compareTime(curr, alarm_) && !playingAzan_)
    {
        playSound(Alarm);
        setAlarm_ = false;
    }
    publishTime(curr, prayerTime, prayerName);
}

void ntp_prayer::presetAlarm(int minute)   
{
      // implement preset alarm
      int value = minute; 
      alarm_ = current_.addMinute(value);
      setAlarm_ = true;
}

void ntp_prayer::decodeDateTime(const char *data, int *timestamp)
{
    // data = "21.02.2023 17:52";
    // skip unnecessary characters and convert chars to integers
    int i, j;
    int buffer[dateBufferSize];
    
    i = j = 0;
    while(j < dateBufferSize)
    {
        int value = data[i] - '0';
        // integer digit cannot be more than 9
        if(value >=0 && value <10)
        {
            buffer[j++] = value;
        }
        i++; 
    }
    
    // 210220231752

    timestamp[0] = buffer[0] * 10 + buffer[1]; // day
    timestamp[1] = buffer[2] * 10 + buffer[3]; // month
    timestamp[2] = buffer[4] * 1000 + buffer[5] * 100 + buffer[6] * 10 + buffer[7]; // year
    timestamp[3] = buffer[8] * 10 + buffer[9]; // hour
    timestamp[4] = buffer[10] * 10 + buffer[11]; // minute
}

void ntp_prayer::setClockButton(const char *data)
{
    int intTime[timestampSize];
    decodeDateTime(data, intTime);
    current_ = Time(intTime[3], intTime[4], 0);
    setTime(intTime[3], intTime[4]);
}