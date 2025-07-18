#ifndef MEDIBOX_ALARM_H
#define MEDIBOX_ALARM_H

#include <Arduino.h>
#include "MediBoxConfig.h"

class MediBoxAlarm {
public:
  static void initAlarm();
  static void ringAlarm(int alarmIndex);
  static void setAlarm(int alarmIndex);
  static void handleAlarmSnooze(int alarmIndex);
  static void handleAlarmCancel(int alarmIndex);
  static void disableAllAlarms();
  static void viewAlarms();
  static void deleteAlarm();
  static bool isAlarmEnabled();
  static int getAlarmHours(int index);
  static int getAlarmMinutes(int index);
  static bool isAlarmTriggered(int index);
  static void setAlarmTriggered(int index, bool triggered); 

private:
  static bool alarmEnabled;
  static int alarmHours[N_ALARMS];
  static int alarmMinutes[N_ALARMS];
  static bool alarmTriggered[N_ALARMS];
  static bool alarmIsTemp[N_ALARMS];
  static int permanentAlarmCount;
  static const int musicalNotes[N_NOTES];
};

#endif