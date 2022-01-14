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

#define EEPROM_PRAYER_METHOD (60)

namespace ROM{

    struct Credential{
        const char* ssid ;
        const char* password;
    };

    struct Location
    {
        String latitude;
        String longitude;
    };
    

    class Manager{
    public:
        /**
         * @brief Construct a new Manager object.
         * An important caveat is that the maximum buffer length needs to be smaller than eRead size \par
         * Plus pay attention to the location of your EEPROM variables which are defined above \par 
         * Make sure your variables don't overwirte each other
         * Maintain enough distance for defining each variable location
         */
        Manager()
        {
            EEPROM.begin(512); 
        }

        /**
         * @brief although it is more appropriate to use double as each argument type, 
         * nodemcu cannot convert them properly. It is much convenient to directly save them as a string format.
         * 
         * @param str_time_zone your current time zone in const string format 
         * @param str_latitude  your latitude in const string format
         * @param str_longitude your longitude in const string format 
         */
        void write_location(const char* str_time_zone,  const char* str_latitude, const char* str_longitude)
        {
            Serial.println("\t[ROM] Writing Data Begin...");
 
            SaveByte(EEPROM_TIME_ZONE_LEN, strlen(str_time_zone));
            SaveString(EEPROM_TIME_ZONE_STR, str_time_zone);
            Serial.println(str_time_zone);

            SaveByte(EEPROM_LATITUDE_LEN, strlen(str_latitude));
            SaveString(EEPROM_LATITUDE_STR, str_latitude);
            Serial.println(str_latitude);

            SaveByte(EEPROM_LONGITUDE_LEN, strlen(str_longitude));
            SaveString(EEPROM_LONGITUDE_STR, str_longitude);
            Serial.println(str_longitude);
            Serial.println("\t[ROM] Writing Data Completed!");
        }

        /**
         * @brief writing prayer method in eeprom e.g., ISNA -> 2 \par 
         * For more information see this website https://aladhan.com/calculation-methods
         * prayer method is so small we can save it as byte 
         * @param method 
         */
        void write_prayer_method(int method)
        {
            SaveByte(EEPROM_PRAYER_METHOD, method);
        }

        String get_prayer_method()
        {
            byte x = ReadByte(EEPROM_PRAYER_METHOD);
            return (String) x;
        }

        /**
         * @brief Get the location of your place
         * Azan clock needs this information to find the prayer times for your specific location 
         * @return Location 
         */
        Location get_location()
        {
            byte x = -2;
            
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
            Serial.println("[ROM] Reading location Completed!");
            return{tempLat, tempLon};
        }

        /**
         * @brief Get the timezone of your place 
         * Smart clock needs this information to calculate offset in time from the ntp server; 
         * @return int 
         */
        int get_timezone()
        {
            byte x = -2;
            x = ReadByte(EEPROM_TIME_ZONE_LEN);
            ReadString(EEPROM_TIME_ZONE_STR, x);
            tempTZ = "";
            for (byte i = 0; i < len; i++)
            tempTZ += eRead[i];
            Serial.println("[ROM] Reading timezone Completed!");
            return tempTZ.toInt();
        }

        void write_credential(const char* ssid, const char* password)
        {
            Serial.println("\t[ROM] Writing Data Begin...");
            SaveByte(1, 10);
            SaveByte(EEPROM_SSID_LEN, strlen(ssid)); // Length of ssid
            SaveByte(EEPROM_PASS_LEN, strlen(password)); // Length of password
            SaveString(EEPROM_SSID_STR, ssid);
            SaveString(EEPROM_PASS_STR, password);
            Serial.println("\t[ROM] Writing Data Complated!");
        }
        Credential get_credential()
        {
            byte x = -2;
            x = ReadByte(1);

            if (x == 10)
            {
                Serial.println("[ROM] Reading wifi credential ...");
                x = ReadByte(EEPROM_SSID_LEN);
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

            Serial.println("[ROM] No credential found!");
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