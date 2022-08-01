#include "button_clock.h"
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "mqtt_client.h"


namespace SMART_CLOCK{
    class ntp_prayer: public ButtonClock::Clock, public mqtt_client{
    public:
        ntp_prayer();
        void repeat();
        void init(); 
    protected:
        Time fetchTime();
        String fetch_content(const char *url);
        void notifyTime(const Time& curr);
        
    };
}