#pragma once 

#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SSID_LEN (2)
#define EEPROM_PASS_LEN (3)
#define EEPROM_SSID_STR (5)
#define EEPROM_PASS_STR (31)

#define EEPROM_TIME_ZONE_LEN (50)
#define EEPROM_TIME_ZONE_STR (55)
#define EEPROM_LATITUDE_LEN (100)
#define EEPROM_LATITUDE_STR (103)
#define EEPROM_LONGITUDE_LEN (120)
#define EEPROM_LONGITUDE_STR (123)

namespace WifiManager{

    struct Credential{
        const char* ssid ;
        const char* password;
    };

    struct Location
    {
        const int timezone;
        const double latitude;
        const double longitude;
    };
    

    class Manager{
    public:
        Manager()
        {
            EEPROM.begin(512); 
        }
        void write_location(const char* str_time_zone,  const char* str_latitude, const char* str_longitude)
        {
            Serial.println("\tWriting Data Begin...");
 
            SaveByte(EEPROM_TIME_ZONE_LEN, strlen(str_time_zone));
            SaveString(EEPROM_TIME_ZONE_STR, str_time_zone);
            Serial.println(str_time_zone);

            SaveByte(EEPROM_LATITUDE_LEN, strlen(str_latitude));
            SaveString(EEPROM_LATITUDE_STR, str_latitude);
            Serial.println(str_latitude);

            SaveByte(EEPROM_LONGITUDE_LEN, strlen(str_longitude));
            SaveString(EEPROM_LONGITUDE_STR, str_longitude);
            Serial.println(str_longitude);

            Serial.println("\tWriting Data Completed!");
        }

        Location get_location()
        {
            byte x = -2;
            x = ReadByte(EEPROM_TIME_ZONE_LEN);
            ReadString(EEPROM_TIME_ZONE_STR, x);
            tempTZ = "";
            for (byte i = 0; i < len; i++)
            tempTZ += eRead[i];

            x = ReadByte(EEPROM_LATITUDE_LEN);
            ReadString(EEPROM_LATITUDE_STR, x);
            tempLat = "";
            for (byte i = 0; i < len; i++)
            tempLat += eRead[i];


            x = ReadByte(EEPROM_LONGITUDE_LEN);
            ReadString(EEPROM_LONGITUDE_STR, x);
            tempLon = "";
            for (byte i = 0; i < len; i++)
            tempLon += eRead[i];

            // Serial.print("lat: \t\t"); Serial.println(tempTZ);
            // Serial.print("long: \t\t"); Serial.println(tempLat);
            // Serial.print("tz: \t\t"); Serial.println(tempLon);
            // Serial.println("+++++++EEPROM++++++++");

            return{tempTZ.toInt(), tempLat.toDouble(), tempLon.toDouble()};
        }

        void write_credential(const char* ssid, const char* password)
        {
            Serial.println("\tWriting Data Begin...");
            SaveByte(1, 10);
            SaveByte(EEPROM_SSID_LEN, strlen(ssid)); // Length of ssid
            SaveByte(EEPROM_PASS_LEN, strlen(password)); // Length of password
            SaveString(EEPROM_SSID_STR, ssid);
            SaveString(EEPROM_PASS_STR, password);
            Serial.println("\tWriting Data Complated!");
        }
        Credential get_credential()
        {
            byte x = -2;
            x = ReadByte(1);

            Serial.print("\t\tValue of byte at adress 1 ="); Serial.println(x);
            if (x == 10)
            {
                Serial.println("\n\tConnecting to Wi-Fi");
                x = ReadByte(EEPROM_SSID_LEN);
                // Serial.print("\t\tValue of byte at adress 2 ="); Serial.println(x);
                ReadString(EEPROM_SSID_STR, x);
                tempS = "";
                for (byte i = 0; i < len; i++)
                tempS += eRead[i];
            
                x = ReadByte(EEPROM_PASS_LEN);
                ReadString(EEPROM_PASS_STR, x);
                tempP = "";
                for (byte i = 0; i < len; i++)
                tempP += eRead[i]; 
                return {tempS.c_str(), tempP.c_str()};               
            }

            Serial.println("No credential found!");
            return {};
        }


    private:
        char eRead[25];
        byte len;
        String tempS, tempP;
        String tempTZ, tempLat, tempLon;
    protected:
        // Saves string do EEPROM
        void SaveString(int startAt, const char* id)
        {
            for (byte i = 0; i <= strlen(id); i++)
            {
                EEPROM.write(i + startAt, (uint8_t) id[i]);
            }
            EEPROM.commit();
        }

        // Reads string from EEPROM
        void ReadString(byte startAt, byte bufor)
        {
            for (byte i = 0; i <= bufor; i++)
            {
                eRead[i] = (char)EEPROM.read(i + startAt);
            }
            len = bufor;
        }
        //Saves byte to EEPROM
        void SaveByte(int startAt, byte val)
        {
            EEPROM.write(startAt, val);
            EEPROM.commit();
        }

        //Reads byte from EEPROM
        byte ReadByte(byte startAt)
        {
            byte Read = -1;
            Read = EEPROM.read(startAt);
            return Read;
        }
    };
}