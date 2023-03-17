#pragma once
#include "ClockDate.h"
#include "ClockTime.h"
#include <time.h>
#include "../data/prayer_table_hex.h"
#define NUM_PRAYERS 5
#define HOURS_PER_DAY 24
#define MILLI_SECOND 1000
#define CST (-6)

class PrayerTime {
public:
  PrayerTime() {

  }
  explicit PrayerTime(const ClockDate &today) {
    updateDate(today);
  }

  void updateDate(const ClockDate &today) {
    // update calander
    int dayInYear = today.numOfDaysIndex() % 365;
    int index = 0;
    for (int i = 0; i < NUM_PRAYERS * 2; i += 2) {
      int tableIndex = (dayInYear * NUM_PRAYERS * 2 + i);
    //      printf("%02u:%02u \n", prayerTableHex[tableIndex], prayerTableHex[tableIndex+1]);
      _timetable[index][0] = prayerTableHex[tableIndex]+ today.isDaylightSaving();
      _timetable[index][1] = prayerTableHex[tableIndex+1];
      ++index;
    }
  }

  void get(ClockTime *prayers) {
    for (int i = 0; i < NUM_PRAYERS; ++i) {
      prayers[i] = ClockTime(_timetable[i][0], _timetable[i][1], 0);
    }
  }

private:
  int _timetable[NUM_PRAYERS][2];
};