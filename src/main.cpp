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




MqttInterface *interface; 
Timer<3, micros> timer;


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
        int isUpdateClock = 1;
        int isUpdateTimer = 0;  
        timer.every(6e7, update_smart_clock, (void *) isUpdateClock);
        timer.every(1e6, update_smart_clock, (void *) isUpdateTimer); 
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
    if(isUpdateClock)
        return interface->update_every_minute(argument);
    else
        return interface->update_frequently(argument);
    return true;
  }
}myclock;


// AzanClock myclock;




void setup() {
  // put your setup code here, to run once:
  // set RX pullup to avoid clicking sound 
  pinMode(D9, INPUT_PULLUP);

  ArduinoOTA.begin();
  myclock.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  myclock.tick();
  
}