#include "button_clock.h"
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "mqtt_client.h"

const int dateBufferSize = 12;
const int timestampSize = 5;

namespace SMART_CLOCK{
    class ntp_prayer: public ButtonClock::Clock, public mqtt_client{
    public:
        ntp_prayer();
        void repeat();
        void init(); 
    protected:
        Time fetchTime();
        String fetch_content(const char *url);
        void notifyTime(const Time& curr, const Time& prayerTime, const String& prayerName);
        void reset(int type) override;
        void presetAlarm(int minute) override;
        void setClockButton(const char *data) override;
        void decodeDateTime(const char *data, int *timestamp);
    };
}