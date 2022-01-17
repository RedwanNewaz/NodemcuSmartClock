#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <arduino-timer.h>
#include <ArduinoOTA.h>
#include <RemoteDebug.h>

#include "smart_clock.h"
#include "rom_manager.h"
#include "azan_clock.h"

#define HOST_NAME "192.168.1.161"
#define USE_ARDUINO_OTA true


#define RESET_PIN   (D3)
#define MINIUTE_PIN (D1)
#define HOUR_PIN    (D2)
#define POWER_PIN   (D0)

/* DIAGRAM 
  clock input: (W-, W+, B-, G+)
  nodemcu input (D3, D1, D2)
    W-    W+    B-    G+
    |     |     |     |
    --------------------
      |     |      |  
      D3    D1     D2 
  Since W+ and B- have multiple use, only 3 transistors are used to make this circuit 
*/


// music clock play azan sound when it is triggered by azan clock 
#ifdef OFFLINE_AZAN
#include "music_clock.h"
MusicClock wav;
#else
// azan will be streamed from the internet 
#include "stream_azan.h"
StreamAzan wav;
#endif 

ROM::Manager manager; 
WiFiUDP ntpUDP;
ButtonClock::Clock clock_button(POWER_PIN, RESET_PIN, HOUR_PIN, MINIUTE_PIN);

// azan clock keep tracks of prayer time based on current date and time 
AzanClock azan_clock;
// smart clock runs every thing and also maintains time synchronization with the ntp server and azan server 
SmartClock smart_clock(ntpUDP, "pool.ntp.org", clock_button, azan_clock);

// remote deubg 
RemoteDebug Debug;
volatile bool initialization = false;

// timer is useful for creating a software clock but it needs synchronization with the ntp clock  
Timer<3, micros> timer;
bool initialize_smart_clock(void *argument);
bool update_smart_clock(void *argument);
bool update_2s_clock(void *argument);
bool repeat_azan_clock(void *argument);



void setup() {
  // start serial for debugging purpose only
  Serial.begin(115200);
  delay(1000);
  // Connecting to Wi-Fi with credential   
  // save personal information to EEPROM 
  // manager.write_credential("your ssid", "your password")
  // manager.write_location("your_tz", "your_lat", "your_lon"); 
  // manager.write_prayer_method(2);// for ISNA 
  Serial.print("Connecting to ");
  auto net = manager.get_credential();
  WiFi.begin(net.ssid, net.password);
  Serial.println(net.ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  // start OTA for software update 
  ArduinoOTA.begin();
  // intialize remote debug 
  Debug.begin(HOST_NAME); // Initialize the WiFi server
  Debug.setResetCmdEnabled(true); // Enable the reset command
  Debug.showProfiler(true); // Profiler (Good to measure times, to optimize codes)
  Debug.showColors(true); // Colors

  // intialize timer 
  timer.in(5e6, initialize_smart_clock);  
  timer.in(3e7, repeat_azan_clock); 
}

void loop() {
  // run timer tick repeatedly to make timer operationalize 
  timer.tick();

  // play azan when count down time is zero
  if(smart_clock.next_prayer() == 0)
  {
    // cancel all pending tasks 
    timer.cancel();
    // this way wav file  does not repeat
    smart_clock.sync_next_prayer_alarm();
    Serial.println("[SmartClock] AZAN time, go to pray ...");
    debugI("[SmartClock] AZAN time, go to pray ...");
    wav.begin();
  }
  // DON'T change this two lines! azan wav won't works anywhere but inside the loop 
  // it needs to be initiated using a timer callback 
  if (wav.isRunning()) {
    if (!wav.loop())
    {
      wav.stop();
      // sync smart clock time 
      smart_clock.sync_clock();
      // make sure there is no pending timer task 
      timer.cancel();
      //resume smart clock timer 
      timer.every(6e7, update_smart_clock);
    }
  }
  // Check for over the air update request and (if present) flash it
  ArduinoOTA.handle();
  // update remote debug handle 
  Debug.handle();
}

// -------------- Functions for timer --------------
/**
 * @brief initialization process invokes the button clock to setup the current time from ntp server  
 * it will also create a timer for updating smart clock internal time in every minute  \par
 */
bool initialize_smart_clock(void *argument)
{
  Serial.println("\n[Main::init] initializing smart clock ");
  smart_clock.setTimeOffset(manager.get_timezone() * 3600);
  smart_clock.reset_clock();
  timer.every(6e7, update_smart_clock);
  Serial.print("[Main::init] Timer pending Task "); Serial.println(timer.size());
  // timer.every(2e6, update_2s_clock); // debuging purposes 
  return false;
}

/**
 * @brief smart clock maintains internal clock counter to update next prayer time \par 
 * smart clock get synchronized with the ntp server @ 12:00 AM every day   * 
 */
bool update_smart_clock(void *argument)
{
  debugI("[SmartClock v2] next prayer is coming in %d minutes", smart_clock.next_prayer());
  smart_clock.update_clock();
  return true;
}

bool update_2s_clock(void *argument)
{
  Serial.println("[Main::debug] updating 2 sec clock");
  return true;
}

/**
 * @brief when the system get initialized for the first time, azan will be played as well \par 
 * it creates a timer task for repeating azan based on the next prayer time \par
 * azan wav needs to load every time 
 */
bool repeat_azan_clock(void *argument)
{
  if(!initialization)
  {
    // test azan for 30s 
    debugI("[Main::debug] azan will play for 30s");
    wav.begin(); 
    initialization = true; 
    return true;  
  }
  // next time when it will come back stop azan 
  wav.desync(); 
  wav.forceStop();
  wav.stop();
  
  debugI("[Main::debug] azan will stop repeating");
  return false;
}