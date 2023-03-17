#include <Arduino.h>
#include "interface/MqttInterface.h"
#include <arduino-timer.h>
#include <ArduinoOTA.h>
#include "clock/ClockNetwork.h"
#include "credential.h"
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#define WIFI_TIMEOUT_MS 2000



MqttInterface *interface; 
Timer<4, micros> timer;


class AzanClock{
public:
    static void init()
    {
        Serial.begin(115200);
        // // join wifi 
        delay(1000);
        Serial.print("Connecting....... ");

        WiFi.begin(net, password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.printf("\n [!!] Connected to = %s \n", net);
        delay(1000);


        // netClock = new ClockNetwork(ntpUDP, "pool.ntp.org");
        // const char * timestamp = netClock->fetchTime().c_str();
        
        const char * timestamp =  "16.03.2023 19:13";
        interface = new MqttInterface(ClockTime(timestamp));
        interface->init(); 
        // delay(1000);
        int isUpdateClock = 0;
        int isUpdateTimer = 1; 
        int isinitSound = 2;  
        timer.every(6e7, update_smart_clock, (void *) isUpdateClock);
        timer.every(1e6, update_smart_clock, (void *) isUpdateTimer); 
        timer.every(15e5, update_smart_clock, (void *) isinitSound); 
    }
    static void tick()
    {
        timer.tick();
        interface->tick();
        interface->client_loop();
    }
protected:
  static bool update_smart_clock(void *argument)
  {
    int isUpdateClock = (int) argument;
    bool repeat = true; 
    switch(isUpdateClock)
    {
      case 0:
        return interface->update_every_minute(argument);
      case 1:
        return interface->update_frequently(argument);
      case 2:
        return interface->init_sound();
    }
    return repeat;
  }
}myclock;


// AzanClock myclock;




void setup() {
  // put your setup code here, to run once:
  // set RX pullup to avoid clicking sound 


  ArduinoOTA.begin();
  myclock.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  myclock.tick();
  
}