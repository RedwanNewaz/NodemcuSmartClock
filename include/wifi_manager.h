#pragma once 

#include <Arduino.h>
#include <EEPROM.h>

namespace WifiManager{

    struct Credential{
        const char* ssid ;
        const char* password;
    };

    class Manager{
    public:
        Manager()
        {
            EEPROM.begin(512); 
        }

        void write_credential(const char* ssid, const char* password)
        {
            Serial.println("\tWriting Data Begin...");
            SaveByte(1, 10);
            SaveByte(2, strlen(ssid)); // Length of ssid
            SaveByte(3, strlen(password)); // Length of password
            SaveString(5, ssid);
            SaveString(31, password);
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
                x = ReadByte(2);
                Serial.print("\t\tValue of byte at adress 2 ="); Serial.println(x);
                ReadString(5, x);
                tempS = "";
                for (byte i = 0; i < len; i++)
                tempS += eRead[i];
            
                x = ReadByte(3);
                ReadString(31, x);
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