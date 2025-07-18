#ifndef MEDIBOX_TIME_H
#define MEDIBOX_TIME_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "MediBoxConfig.h"

class MediBoxTime {
public:
  static void initTime();
  static void updateTimeWithCheckAlarm();
  static void updateTime();
  static int getDays();
  static String getDayOfWeek();
  static int getHours();
  static int getMinutes();
  static int getSeconds();
  static int getUtcOffset();
  static void setUtcOffset(int hours, int mins);

private:
  static int days;
  static String dayOfWeek;
  static int hours;
  static int minutes;
  static int seconds;
  static int utcOffset;
  static int offsetHours;
  static int offsetMins;
  static unsigned long lastUpdateTime;
  static const unsigned long UPDATE_INTERVAL;
  static const String DAYS_OF_WEEK[7];
};

#endif