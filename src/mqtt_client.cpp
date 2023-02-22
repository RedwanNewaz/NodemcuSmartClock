#include "mqtt_client.h"


using namespace SMART_CLOCK;

mqtt_client::mqtt_client()
{
    client_ = new PubSubClient(wifiClient_);
    server_ = new IPAddress(192, 168, 1, 1);
    setAlarm_ = false;
}

void mqtt_client::init()
{
    client_->setServer(*server_, 1883);
    client_->setCallback([=](char* topic, byte* payload, unsigned int length){this->callback(topic, payload, length);});
}

void mqtt_client::callback(char* topic, byte* payload, unsigned int length)
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
      alarm_.hour = data.toInt();
    else if(String(topic).equals(minuteTopic))
      alarm_.minute = data.toInt();
    else if(String(topic).equals(startTopic))
      setAlarm_ = !setAlarm_;
    else if(String(topic).equals(resetTopic))
      reset(data.toInt());
    else if(String(topic).equals(presetTopic))
      presetAlarm(data.toInt());
    else if(String(topic).equals(syncTopic))
      setClockButton(data.c_str());
 
     
 
}

void mqtt_client::reconnect()
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


void mqtt_client::mqttLoop()
{
    if (!client_->connected()) {
      reconnect();
    }
    client_->loop();
}

void mqtt_client::publishTime(const Time& time, const Time& prayerTime, const String& prayerName)
{
    
    String alarmTime = (setAlarm_) ? alarm_.toString() : String(setAlarm_);
    client_->publish(pubTime, time.toString().c_str());
    client_->publish(pubAlarm, alarmTime.c_str());
    client_->publish(pubPrayerTime, prayerTime.toString().c_str());
    client_->publish(pubPrayerName, prayerName.c_str());
}