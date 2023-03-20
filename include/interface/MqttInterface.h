#include "config.h"
#include "MusicInterface.h"
#include <WiFiClient.h>
#include <PubSubClient.h>

class MqttInterface: public MusicInterface
{
            
private:
    WiFiClient wifiClient_;
    IPAddress *server_;
    PubSubClient *client_;
    int alarm_time_[2];
public:
    MqttInterface()
    {
        alarm_time_[0] = alarm_time_[1] = 0;
    }

    MqttInterface(const ClockTime &time):MusicInterface(time)
    {
        alarm_time_[0] = alarm_time_[1] = 0;
    }

    void init()
    {
        client_ = new PubSubClient(wifiClient_);
        server_ = new IPAddress(MQTT_SERVER_ADDRESS[0], MQTT_SERVER_ADDRESS[1], MQTT_SERVER_ADDRESS[2], MQTT_SERVER_ADDRESS[3]);
        
        client_->setServer(*server_, 1883);
        client_->setCallback([=](char* topic, byte* payload, unsigned int length){this->callback(topic, payload, length);});
    }

    void callback(char* topic, byte* payload, unsigned int length)
    {

        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        String data = "";
        for (int i = 0; i < length; i++) {
            data += (char)payload[i];
        }
        Serial.println(data);

        // set values 
        if(String(topic).equals(hourTopic))
        alarm_time_[0] = data.toInt();
        else if(String(topic).equals(minuteTopic))
        alarm_time_[1] = data.toInt();
        else if(String(topic).equals(startTopic))
        {
            ClockTime alarm(alarm_time_[0], alarm_time_[1], 0);
            setAlarmTime(alarm);
        }
        // else if(String(topic).equals(resetTopic))
        // reset(data.toInt());
        else if(String(topic).equals(presetTopic))
        {
            int inMinute = data.toInt();
            ClockTime alarm = _currentTime.addMinute(inMinute);
            setAlarmTime(alarm);
        }
        else if(String(topic).equals(syncTopic))
        {
            ClockTime current(data.c_str());
            reset(current);
        }
    
        
    
    }

    void reconnect()
    {
        // Loop until we're reconnected
        while (!client_->connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client_->connect("arduinoClient")) {
            Serial.println("Connected!!");
            // ... and resubscribe
            client_->subscribe(hourTopic);
            client_->subscribe(minuteTopic);
            client_->subscribe(startTopic);
            client_->subscribe(resetTopic);
            client_->subscribe(presetTopic);
            client_->subscribe(syncTopic);
        } 
        else 
        {
            Serial.print("failed, rc=");
            Serial.print(client_->state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
        }
    }


    void client_loop()
    {
        if (!client_->connected()) {
            reconnect();
        }
        client_->loop();
        // MusicInterface::tick();
    }

    String paddedStr(int val) const 
    {
        String str = String(val);
        if(str.length() == 1)
            str = "0" + str;
        return str; 
    }

    String toString(const ClockTime& time) const
    {
        String format = paddedStr(time.getHour()) + ":" + paddedStr(time.getMinute());
        return format;
    }



    void publishTime(const ClockTime& time, const ClockTime& prayerTime, const ClockTime& alarmTime, const char* prayerName)
    {
        client_->publish(pubTime, toString(time).c_str());
        client_->publish(pubAlarm, toString(alarmTime).c_str());
        client_->publish(pubPrayerTime, toString(prayerTime).c_str());
        client_->publish(pubPrayerName, prayerName);
    }
};

