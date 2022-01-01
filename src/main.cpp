#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <arduino-timer.h>

#include "smart_clock.h"
#include "wifi_manager.h"
#include "azan_clock.h"
#include "music_clock.h"


#define RESET_PIN   (D1)
#define MINIUTE_PIN (D2)
#define HOUR_PIN    (D3)
#define POWER_PIN   (D0)

// WifiManager::Location loc{my_timezone, my_latitude, my_longitude};

WifiManager::Manager manager; 
WiFiUDP ntpUDP;
ButtonClock::Clock clock_button(POWER_PIN, RESET_PIN, HOUR_PIN, MINIUTE_PIN);

AzanClock azan_clock;
MusicClock wav;
SmartClock smart_clock(ntpUDP, "pool.ntp.org", clock_button, azan_clock);

Timer<3, micros> timer;
bool initialize_smart_clock(void *argument);
bool update_smart_clock(void *argument);
bool update_2s_clock(void *argument);
bool repeat_azan_clock(void *argument);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);

  // save personal information to EEPROM 
  // manager.write_credential("your ssid", "your password")
  // manager.write_location("your_tz", "your_lat", "your_lon"); 

  Serial.print("Connecting to ");
  auto net = manager.get_credential();
  WiFi.begin(net.ssid, net.password);
  Serial.println(net.ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  // wav->begin();
   // Connect to Wi-Fi  
  timer.in(5e6, initialize_smart_clock);  
  timer.in(3e7, repeat_azan_clock); 
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.tick();
  if (wav.isRunning()) {
    if (!wav.loop()) wav.stop();
  }
}

// -------------- Functions for timer --------------
bool initialize_smart_clock(void *argument)
{
  Serial.println("Resetting clock ");
  auto loc = manager.get_location();
  azan_clock.set_location(loc);

  smart_clock.setTimeOffset(loc.timezone * 3600);
  smart_clock.reset_clock();
  timer.every(6e7, update_smart_clock);
  Serial.print("Timer pending Task "); Serial.println(timer.size());
  // timer.every(2e6, update_2s_clock); // debuging purposes 


  return false;
}

bool update_smart_clock(void *argument)
{
  smart_clock.update_clock();
  return true;
}

bool update_2s_clock(void *argument)
{
  Serial.println("updating 2 sec clock");
  return true;
}


bool repeat_azan_clock(void *argument)
{
  Serial.print("Azan will be repeated in ");
  auto wait_time = smart_clock.next_prayer() * 60 * 1e6;
  // auto wait_time = 1 * 60 * 1e6;
  timer.in(wait_time, repeat_azan_clock); 
  wav.begin();
  Serial.println(wait_time);
  Serial.print("Timer pending Task "); Serial.println(timer.size());
  return false;
}