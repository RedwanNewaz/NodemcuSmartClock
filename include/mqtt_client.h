#pragma once 
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "config.h"

namespace SMART_CLOCK{
    class mqtt_client{
        public:
            mqtt_client();
            void mqttLoop();
            void init();
        protected:
             void callback(char* topic, byte* payload, unsigned int length);
             void reconnect();
             Time alarm_; 
             void publishTime(const Time& time, const Time& prayerTime, const String& prayerName);
             bool setAlarm_;
        private:
            WiFiClient wifiClient_;
            IPAddress *server_;
            PubSubClient *client_;
            
            
    };
}