#include <Arduino.h>
#include <math.h>

// #include <String.h>
/*-------------------------- In the name of God ----------------------------*\


/* Arduino class library  to calculate Islamic prayer times 					*/
/* This is ported  by Shajeeb T Muhammad from the library published at PrayTimes.org 	*/
/* Refer https://github.com/shajeebtm for usage details with example.
	
/*
-----------------------Original Copyright Block --------------------------------

Copyright (C) 2007-2010 PrayTimes.org

Developed By: Mohammad Ebrahim Mohammadi Panah <ebrahim at mohammadi dot ir>
Based on a JavaScript Code By: Hamid Zarrabi-Zadeh

License: GNU LGPL v3.0

TERMS OF USE:
    Permission is granted to use this code, with or
    without modification, in any website or application
    provided that credit is given to the original work
    with a link back to PrayTimes.org.

This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY.

PLEASE DO NOT REMOVE THIS COPYRIGHT BLOCK.

------------------------------------------------------------------------------
*/



class PrayerTimes
{
  public:
    enum
    {
      VERSION_MAJOR = 1,
      VERSION_MINOR = 0,
    };
    /* --------------------- User Interface ----------------------- */

    // Calculation Methods
    enum CalculationMethod
    {
      Jafari,   // Ithna Ashari
      Karachi,  // University of Islamic Sciences, Karachi
      ISNA,     // Islamic Society of North America (ISNA)
      MWL,      // Muslim World League (MWL)
      Makkah,   // Umm al-Qura, Makkah
      Egypt,    // Egyptian General Authority of Survey
      Custom,   // Custom Setting

      CalculationMethodsCount
    };

    // Juristic Methods
    enum JuristicMethod
    {
      Shafii,    // Shafii (standard)
      Hanafi,    // Hanafi
    };

    // Adjusting Methods for Higher Latitudes
    enum AdjustingMethod
    {
      None,       // No adjustment
      MidNight,   // middle of night
      OneSeventh, // 1/7th of night
      AngleBased, // angle/60th of night
    };

    // Time IDs
    enum TimeID
    {
      Fajr,
      Sunrise,
      Dhuhr,
      Asr,
      Sunset,
      Maghrib,
      Isha,

      TimesCount
    };


    /* -------------------- Interface Functions -------------------- */

    PrayerTimes(CalculationMethod calc_method = Jafari,
                JuristicMethod asr_juristic = Shafii,
                AdjustingMethod adjust_high_lats = None,
                float dhuhr_minutes = 0)
      : calc_method(calc_method)
      , asr_juristic(asr_juristic)
      , adjust_high_lats(adjust_high_lats)
      , dhuhr_minutes(dhuhr_minutes)
    {
      method_params[Jafari]  = MethodConfig(16.0, false, 4.0, false, 14.0); // Jafari
      method_params[Karachi] = MethodConfig(18.0, true,  0.0, false, 18.0); // Karachi
      method_params[ISNA]    = MethodConfig(15.0, true,  0.0, false, 15.0); // ISNA
      method_params[MWL]     = MethodConfig(18.0, true,  0.0, false, 17.0); // MWL
      method_params[Makkah]  = MethodConfig(19.0, true,  0.0, true,  90.0); // Makkah
      method_params[Egypt]   = MethodConfig(19.5, true,  0.0, false, 17.5); // Egypt
      method_params[Custom]  = MethodConfig(18.0, true,  0.0, false, 17.0); // Custom
    }




    /* return prayer times for a given date */
    void get_prayer_times(int year, int month, int day, float _latitude, float _longitude, float _timezone, bool _dst_on, String _calc_method, String _asr_method, String _my_high_lats_method, float times[])
    {
      latitude = _latitude;
      longitude = _longitude;
      timezone = _timezone + _dst_on;
      // if (_dst_on) {
      //   timezone+=1; // FIXME changed from +1 -> -1
      // }

      // Fix calculation method
      if (_calc_method.equalsIgnoreCase("ISNA")) {
          set_calc_method(ISNA);
      } else if (_calc_method.equalsIgnoreCase("Karachi")) {
        set_calc_method(Karachi);
      } else if (_calc_method.equalsIgnoreCase("MWL")) {
        set_calc_method(MWL);
      } else if (_calc_method.equalsIgnoreCase("Jafari")) {
        set_calc_method(Jafari);
      } else if (_calc_method.equalsIgnoreCase("Makkah")) {
        set_calc_method(Makkah);
      } else if (_calc_method.equalsIgnoreCase("Egypt")) {
        set_calc_method(Egypt);
      } else if (_calc_method.equalsIgnoreCase("Custom")) {
        set_calc_method(Custom);
      }

      // Fix Asr calculation method
      if (_asr_method.equalsIgnoreCase("Shafii")) {
          set_asr_method(Shafii);
      } else if (_asr_method.equalsIgnoreCase("Hanafi")) {
        set_asr_method(Hanafi);
      }

      // Fix high latitude adjustment method
      if (_my_high_lats_method.equalsIgnoreCase("None")) {
          set_high_lats_adjust_method(None);
      } else if (_my_high_lats_method.equalsIgnoreCase("MidNight")) {
          set_high_lats_adjust_method(MidNight);
      } else if (_my_high_lats_method.equalsIgnoreCase("OneSeventh")) {
          set_high_lats_adjust_method(OneSeventh);
      } else if (_my_high_lats_method.equalsIgnoreCase("AngleBased")) {
          set_high_lats_adjust_method(AngleBased);
      }

      
      julian_date = get_julian_date(year, month, day) - longitude / (float) (15 * 24);
      compute_day_times(times);
    }


    /* set the calculation method  */
    void set_calc_method(CalculationMethod method_id)
    {
      calc_method = method_id;
    }

    /* set the juristic method for Asr */
    void set_asr_method(JuristicMethod method_id)
    {
      asr_juristic = method_id;
    }

    /* set adjusting method for higher latitudes */
    void set_high_lats_adjust_method(AdjustingMethod method_id)
    {
      adjust_high_lats = method_id;
    }

    /* set the angle for calculating Fajr */
    void set_fajr_angle(float angle)
    {
      method_params[Custom].fajr_angle = angle;
      calc_method = Custom;
    }

    /* set the angle for calculating Maghrib */
    void set_maghrib_angle(float angle)
    {
      method_params[Custom].maghrib_is_minutes = false;
      method_params[Custom].maghrib_value = angle;
      calc_method = Custom;
    }

    /* set the angle for calculating Isha */
    void set_isha_angle(float angle)
    {
      method_params[Custom].isha_is_minutes = false;
      method_params[Custom].isha_value = angle;
      calc_method = Custom;
    }

    /* set the minutes after mid-day for calculating Dhuhr */
    void set_dhuhr_minutes(float minutes)
    {
      dhuhr_minutes = minutes;
    }

    /* set the minutes after Sunset for calculating Maghrib */
    void set_maghrib_minutes(float minutes)
    {
      method_params[Custom].maghrib_is_minutes = true;
      method_params[Custom].maghrib_value = minutes;
      calc_method = Custom;
    }

    /* set the minutes after Maghrib for calculating Isha */
    void set_isha_minutes(float minutes)
    {
      method_params[Custom].isha_is_minutes = true;
      method_params[Custom].isha_value = minutes;
      calc_method = Custom;
    }

    /* to get the name of  the prayer */
    String get_prayer_name(int prayer)
    {
	return PrayerNames[prayer];

    }

    /* get hours and minutes parts of a float time */
    static void get_float_time_parts(float time, int& hours, int& minutes)
    {
      time = fix_hour(time + 0.5 / 60);   // add 0.5 minutes to round
      hours = floor(time);
      minutes = floor((time - hours) * 60);
    }

    /* convert float hours to 24h format */

    static String float_time_to_time24(float time)
    {
      // if (isnan(time))
      // return String;
      int hours, minutes;
      get_float_time_parts(time, hours, minutes);
      return two_digits_format(hours) + ':' + two_digits_format(minutes);
    }


    /* convert float hours to 12h format */
    /* with AM or PM suffix */

    static String float_time_to_time12(float time, bool no_suffix = false)
    {
      //if (isnan(time))
      //return std::string();
      int hours, minutes;
      get_float_time_parts(time, hours, minutes);
      const char* suffix = hours >= 12 ? " PM" : " AM";
      hours = (hours + 12 - 1) % 12 + 1;
      return int_to_string(hours) + ':' + two_digits_format(minutes) + (no_suffix ? "" : suffix);
    }


    /* convert float hours to 12h format with no suffix */
    /* without AM or PM suffix */

    static String float_time_to_time12ns(float time)
    {
      return float_time_to_time12(time, true);
    }


    /* convert hh:mm 12hours format to hh.mm 24hr format, useful for time comparsion */
    /* Added by by Shajeeb     */
        static float time12_to_float_time(int h, int m , bool pm)
        {
                float d;
                if (pm) {
                  h += 12;
                  if (h == 24) {       // afternoon 12'o clock means 12 hours
                    h -= 12;
                  }
                } else {
                  if (h == 12 ) {     // night 12'o clock means 00 hours
                    h = 0;
                  }
                }
                d = (float)m / 60;
                d = d + (float)h;

                return d;
        }




  private:
    // Prayer Names
    String PrayerNames[TimesCount] = {"Fajr", "Sunrise", "Dhuhr", "Asr", "Sunset", "Maghrib", "Isha"};

    /* ------------------- Calc Method Parameters -------------------- */

    struct MethodConfig
    {
      MethodConfig()
      {
      }

      MethodConfig(float fajr_angle,
                   bool maghrib_is_minutes,
                   float maghrib_value,
                   bool isha_is_minutes,
                   float isha_value)
        : fajr_angle(fajr_angle)
        , maghrib_is_minutes(maghrib_is_minutes)
        , maghrib_value(maghrib_value)
        , isha_is_minutes(isha_is_minutes)
        , isha_value(isha_value)
      {
      }

      float fajr_angle;
      bool   maghrib_is_minutes;
      float maghrib_value;   // angle or minutes
      bool   isha_is_minutes;
      float isha_value;    // angle or minutes
    };

    /* ---------------------- Calculation Functions ----------------------- */

    /* References: */
    /* http://www.ummah.net/astronomy/saltime   */
    /* http://aa.usno.navy.mil/faq/docs/SunApprox.html */

    // typedef std::pair<float, float> floatPair;

    /* compute declination angle of sun and equation of time */
    void sun_position(float jd, float &a , float &b)
    {
      float d = jd - 2451545.0;
      float g = fix_angle(357.529 + 0.98560028 * d);
      float q = fix_angle(280.459 + 0.98564736 * d);
      float l = fix_angle(q + 1.915 * dsin(g) + 0.020 * dsin(2 * g));

      // float r = 1.00014 - 0.01671 * dcos(g) - 0.00014 * dcos(2 * g);
      float e = 23.439 - 0.00000036 * d;

      float dd = darcsin(dsin(e) * dsin(l));
      float ra = darctan2(dcos(e) * dsin(l), dcos(l)) / 15.0;
      ra = fix_hour(ra);
      float eq_t = q / 15.0 - ra;
      a = dd ;
      b = eq_t;

      
      // return floatPair(dd, eq_t);
    }

    /* compute equation of time */
    float equation_of_time(float jd)
    {
      float a , b;
      sun_position (jd, a , b);
      return b;
    }

    /* compute declination angle of sun */
    float sun_declination(float jd)
    {
        float a , b;
        sun_position (jd, a , b);
        return a;
    }

    /* compute mid-day (Dhuhr, Zawal) time */
    float compute_mid_day(float _t)
    {
      float t = equation_of_time(julian_date + _t);
      float z = fix_hour(12 - t);
      return z;
    }

    /* compute time for a given angle G */
    float compute_time(float g, float t)
    {
      float d = sun_declination(julian_date + t);
      float z = compute_mid_day(t);
      float v = 1.0 / 15.0 * darccos((-dsin(g) - dsin(d) * dsin(latitude)) / (dcos(d) * dcos(latitude)));
      return z + (g > 90.0 ? - v :  v);
    }

    /* compute the time of Asr */
    float compute_asr(int step, float t)  // Shafii: step=1, Hanafi: step=2
    {
      float d = sun_declination(julian_date + t);
      float g = -darccot(step + dtan(fabs(latitude - d)));
      return compute_time(g, t);
    }

    /* ---------------------- Compute Prayer Times ----------------------- */

    // array parameters must be at least of size TimesCount

    /* compute prayer times at given julian date */
    void compute_times(float times[])
    {
      day_portion(times);

      times[Fajr]    = compute_time(180.0 - method_params[calc_method].fajr_angle, times[Fajr]);
      times[Sunrise] = compute_time(180.0 - 0.833, times[Sunrise]);
      times[Dhuhr]   = compute_mid_day(times[Dhuhr]);
      times[Asr]     = compute_asr(1 + asr_juristic, times[Asr]);
      times[Sunset]  = compute_time(0.833, times[Sunset]);
      times[Maghrib] = compute_time(method_params[calc_method].maghrib_value, times[Maghrib]);
      times[Isha]    = compute_time(method_params[calc_method].isha_value, times[Isha]);
    }


    /* compute prayer times at given julian date */
    void compute_day_times(float times[])
    {
      float default_times[] = { 5, 6, 12, 13, 18, 18, 18 };    // default times
      for (int i = 0; i < TimesCount; ++i)
        times[i] = default_times[i];

      for (int i = 0; i < NUM_ITERATIONS; ++i)
        compute_times(times);

      adjust_times(times);
    }


    /* adjust times in a prayer time array */
    void adjust_times(float times[])
    {
      for (int i = 0; i < TimesCount; ++i)
        times[i] += timezone - longitude / 15.0;
      times[Dhuhr] += dhuhr_minutes / 60.0;   // Dhuhr
      if (method_params[calc_method].maghrib_is_minutes)    // Maghrib
        times[Maghrib] = times[Sunset] + method_params[calc_method].maghrib_value / 60.0;
      if (method_params[calc_method].isha_is_minutes)   // Isha
        times[Isha] = times[Maghrib] + method_params[calc_method].isha_value / 60.0;

      if (adjust_high_lats != None)
        adjust_high_lat_times(times);
    }

    /* adjust Fajr, Isha and Maghrib for locations in higher latitudes */
    void adjust_high_lat_times(float times[])
    {
      float night_time = time_diff(times[Sunset], times[Sunrise]);   // sunset to sunrise

      // Adjust Fajr
      float fajr_diff = night_portion(method_params[calc_method].fajr_angle) * night_time;
      if (isnan(times[Fajr]) || time_diff(times[Fajr], times[Sunrise]) > fajr_diff)
        times[Fajr] = times[Sunrise] - fajr_diff;

      // Adjust Isha
      float isha_angle = method_params[calc_method].isha_is_minutes ? 18.0 : method_params[calc_method].isha_value;
      float isha_diff = night_portion(isha_angle) * night_time;
      if (isnan(times[Isha]) || time_diff(times[Sunset], times[Isha]) > isha_diff)
        times[Isha] = times[Sunset] + isha_diff;

      // Adjust Maghrib
      float maghrib_angle = method_params[calc_method].maghrib_is_minutes ? 4.0 : method_params[calc_method].maghrib_value;
      float maghrib_diff = night_portion(maghrib_angle) * night_time;
      if (isnan(times[Maghrib]) || time_diff(times[Sunset], times[Maghrib]) > maghrib_diff)
        times[Maghrib] = times[Sunset] + maghrib_diff;
    }


    /* the night portion used for adjusting times in higher latitudes */
    float night_portion(float angle)
    {
      switch (adjust_high_lats)
      {
        case AngleBased:
          return angle / 60.0;
        case MidNight:
          return 1.0 / 2.0;
        case OneSeventh:
          return 1.0 / 7.0;
        default:
          // Just to return something!
          // In original library nothing was returned
          // Maybe I should throw an exception
          // It must be impossible to reach here
          return 0;
      }
    }

    /* convert hours to day portions  */
    void day_portion(float times[])
    {
      for (int i = 0; i < TimesCount; ++i)
        times[i] /= 24.0;
    }

    /* ---------------------- Misc Functions ----------------------- */

    /* compute the difference between two times  */
    static float time_diff(float time1, float time2)
    {
      return fix_hour(time2 - time1);
    }


    static String  int_to_string(int num)
    {
      char tmp[16];
      tmp[0] = '\0';
      sprintf(tmp, "%d", num);
      return String(tmp);
    }





    // /* add a leading 0 if necessary */

    static String two_digits_format(int num)
    {
      char tmp[16];
      tmp[0] = '\0';
      sprintf(tmp, "%2.2d", num);
      return String(tmp);
    }



    /* ---------------------- Julian Date Functions ----------------------- */

    /* calculate julian date from a calendar date */
    float get_julian_date(int year, int month, int day)
    {
      if (month <= 2)
      {
        year -= 1;
        month += 12;
      }

      float a = floor(year / 100.0);
      float b = 2 - a + floor(a / 4.0);

      return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + b - 1524.5;
    }


    /* ---------------------- Trigonometric Functions ----------------------- */

    /* degree sin */
    static float dsin(float d)
    {
      return sin(deg2rad(d));
    }

    /* degree cos */
    static float dcos(float d)
    {
      return cos(deg2rad(d));
    }

    /* degree tan */
    static float dtan(float d)
    {
      return tan(deg2rad(d));
    }

    /* degree arcsin */
    static float darcsin(float x)
    {
      return rad2deg(asin(x));
    }

    /* degree arccos */
    static float darccos(float x)
    {
      return rad2deg(acos(x));
    }

    /* degree arctan */
    static float darctan(float x)
    {
      return rad2deg(atan(x));
    }

    /* degree arctan2 */
    static float darctan2(float y, float x)
    {
      return rad2deg(atan2(y, x));
    }

    /* degree arccot */
    static float darccot(float x)
    {
      return rad2deg(atan(1.0 / x));
    }

    /* degree to radian */
    static float deg2rad(float d)
    {
      return d * M_PI / 180.0;
    }

    /* radian to degree */
    static float rad2deg(float r)
    {
      return r * 180.0 / M_PI;
    }

    /* range reduce angle in degrees. */
    static float fix_angle(float a)
    {
      a = a - 360.0 * floor(a / 360.0);
      a = a < 0.0 ? a + 360.0 : a;
      return a;
    }

    /* range reduce hours to 0..23 */
    static float fix_hour(float a)
    {
      a = a - 24.0 * floor(a / 24.0);
      a = a < 0.0 ? a + 24.0 : a;
      return a;
    }

  private:
    /* ---------------------- Private Variables -------------------- */

    MethodConfig method_params[CalculationMethodsCount];

    CalculationMethod calc_method;    // caculation method
    JuristicMethod asr_juristic;    // Juristic method for Asr
    AdjustingMethod adjust_high_lats; // adjusting method for higher latitudes
    float dhuhr_minutes;   // minutes after mid-day for Dhuhr


    float julian_date;

    /* --------------------- Technical Settings -------------------- */

    static const int NUM_ITERATIONS = 1;    // number of iterations needed to compute times

  protected:
    float latitude;
    float longitude;
    float timezone;
};

