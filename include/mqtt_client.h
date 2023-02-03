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
            /// reset(0) reinitialize base::clock 
            /// reset(1) reinitilize button::clock only
            /// use reset(1) when internet (mqtt) time is accurate but clock display wrong time
            /// when mqtt time is also wrong then use any number to reset the base clock and button clock
             virtual void reset(int type) = 0;
             virtual void presetAlarm(int minute) = 0;
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