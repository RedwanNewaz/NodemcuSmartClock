#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "smart_clock.h"
#include <arduino-timer.h>
#include "wifi_manager.h"

#define RESET_PIN   (D1)
#define MINIUTE_PIN (D2)
#define HOUR_PIN    (D3)
#define POWER_PIN   (D4)

WifiManager::Manager manager; 


const int my_timezone = -5;
WiFiUDP ntpUDP;
ButtonClock::Clock clock_button(POWER_PIN, RESET_PIN, HOUR_PIN, MINIUTE_PIN);
SmartClock smart_clock(ntpUDP, "pool.ntp.org", clock_button);

Timer<1, micros> timer;
bool initialize_smart_clock(void *argument);
bool update_smart_clock(void *argument);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Connecting to ");
  auto net = manager.get_credential();
  WiFi.begin(net.ssid, net.password);
  Serial.println(net.ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 

   // Connect to Wi-Fi

  smart_clock.setTimeOffset(my_timezone * 3600);
  timer.in(5e6, initialize_smart_clock);
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.tick();
}

// -------------- Functions for timer --------------
bool initialize_smart_clock(void *argument)
{
  Serial.println("Resetting clock ");
  smart_clock.reset_clock();
  timer.every(6e7, update_smart_clock);
  return false;
}

bool update_smart_clock(void *argument)
{
  smart_clock.update_clock();
  return true;
}
