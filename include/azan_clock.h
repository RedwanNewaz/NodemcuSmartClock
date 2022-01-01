#pragma once 
#include <Arduino.h>
#include <PrayerTimes.h>
#define NUM_DAILY_PRAYERS (5)

class AzanClock: public PrayerTimes{
public:
    AzanClock()
    {

    }
    AzanClock(double _latitude, double _longitude, double _timezone)
    {
        //TODO read this information from eeprom 
        latitude = _latitude;
        longitude = _longitude;
        original_tz_ = _timezone;
    }
    template<typename T>
    void set_location(const T &stat)
    {
        latitude = stat.latitude;
        longitude = stat.longitude; 
        timezone = stat.timezone;
        Serial.print("lat: \t\t"); Serial.println(latitude);
        Serial.print("long: \t\t"); Serial.println(longitude);
        Serial.print("tz: \t\t"); Serial.println(timezone);
    }

    void update_clock(int year, int month, int day)
    {
        // TODO automatically compute these
        String my_calc_method = "ISNA";
        String my_asr_method = "Shafii";
        String my_high_lats_method = "None";
        bool my_dst_on = true; 
        double my_prayer_times[TimesCount];
        
        get_prayer_times(year, month, day, latitude , longitude , original_tz_ , my_dst_on, my_calc_method, my_asr_method, my_high_lats_method, my_prayer_times );

        int hours, minutes;
        int count = 0; 
        int avoid_indexes[2] = {1 , 4}; // these are sunrise and sunset times  
        
        for (int i = 0; i < TimesCount; i++)
        {
            if(i == avoid_indexes[0] || i == avoid_indexes[1])
                continue;
            float time = my_prayer_times[i];
            get_float_time_parts(time, hours, minutes);
            daily_prayer_names_[count] = get_prayer_name(i);
            daily_prayer_times_[count++] = hours * 60 + minutes;
        }
        show_azan_table(my_prayer_times);
    }

    void show_azan_table(const double *my_prayer_times)
    {
        Serial.print("*******  Prayer Times for the date : ");
        Serial.println("Prayer\t\tNumberic-Time\t24Hr-Format\t12Hr-Format");
        Serial.println("----------------------------------------------------------");
        String inStringForm;
        for (int i = 0; i < TimesCount; ++i) {
            Serial.print(get_prayer_name(i)); Serial.print("\t\t");
            Serial.print(my_prayer_times[i]); Serial.print("\t\t");
            inStringForm = float_time_to_time24 (my_prayer_times[i]);
            Serial.print(inStringForm); ; Serial.print("\t\t");
            inStringForm = float_time_to_time12 (my_prayer_times[i]);
            Serial.println(inStringForm);
        }

        Serial.println("\n");
    }

    int next_prayer_in_minutes(int currentTimeInMin)
    {

        for (int i = 0; i < NUM_DAILY_PRAYERS; i++)
        {
            if(daily_prayer_times_[i] > currentTimeInMin)
            {
                Serial.print("Next prayer is ");
                Serial.println(daily_prayer_names_[i]);
                return daily_prayer_times_[i] - currentTimeInMin; 
            }
        }

        return daily_prayer_times_[0] - currentTimeInMin + 24 * 60;
        

    }

private:
    int daily_prayer_times_[NUM_DAILY_PRAYERS];
    String daily_prayer_names_[NUM_DAILY_PRAYERS];
    int original_tz_;
    
    

};