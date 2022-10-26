#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "smart_clock.h"
#include <arduino-timer.h>
#include <ArduinoOTA.h>

SMART_CLOCK::ntp_prayer prayer_clock; 
Timer<3, micros> timer;

bool update_smart_clock(void *argument)
{
  int isUpdateClock = (int) argument;
  if(isUpdateClock == 1)
    prayer_clock.updateClock();
  else
    prayer_clock.updateTimer();
  
  return true;
}


void setup() {
  // put your setup code here, to run once:
  // start serial for debugging purpose only
  Serial.begin(115200);
  delay(1000);
  Serial.print("Connecting to ");

  WiFi.begin(net, password);
  Serial.println(net);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(1000);
  // start OTA for software update 
  ArduinoOTA.begin();
  prayer_clock.init();
  int isUpdateClock = 1;
  int isUpdateTimer = 0; 

  // delay(5000);
  
  timer.every(6e7, update_smart_clock, (void *) isUpdateClock);
  timer.every(1e7, update_smart_clock, (void *) isUpdateTimer);
}

void loop() {
  // put your main code here, to run repeatedly:
  prayer_clock.repeat();
  timer.tick();
  ArduinoOTA.handle();
}