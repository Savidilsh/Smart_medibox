#include "MediBoxTime.h"
#include "MediBoxDisplay.h"
#include "MediBoxAlarm.h"
#include "MediBoxMenu.h"
#include "MediBoxSensor.h"

int MediBoxTime::days = 0;
String MediBoxTime::dayOfWeek = "";
int MediBoxTime::hours = 0;
int MediBoxTime::minutes = 0;
int MediBoxTime::seconds = 0;
int MediBoxTime::utcOffset = 0;
int MediBoxTime::offsetHours = 5;
int MediBoxTime::offsetMins = 30;
unsigned long MediBoxTime::lastUpdateTime = 0;
const unsigned long MediBoxTime::UPDATE_INTERVAL = 1000;
const String MediBoxTime::DAYS_OF_WEEK[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void MediBoxTime::initTime() {
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    MediBoxDisplay::display.clearDisplay();
    MediBoxDisplay::printLine("Connecting to WIFI", 0, 0, 1);
  }
  utcOffset = (offsetHours * 3600) + (offsetMins * 60);
  configTime(utcOffset, UTC_OFFSET_DST, NTP_SERVER);
}

void MediBoxTime::updateTimeWithCheckAlarm() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    updateTime();
    lastUpdateTime = currentTime;

    if (MediBoxMenu::getCurrentState() == HOME_SCREEN) {
      MediBoxDisplay::displayTime(days, dayOfWeek, hours, minutes, seconds,
                                  MediBoxSensor::getTemperature(), MediBoxSensor::getHumidity(),
                                  MediBoxAlarm::isAlarmEnabled());
    }

    if (MediBoxAlarm::isAlarmEnabled()) {
      for (int i = 0; i < N_ALARMS; i++) {
        if (!MediBoxAlarm::isAlarmTriggered(i) && MediBoxAlarm::getAlarmHours(i) == hours && MediBoxAlarm::getAlarmMinutes(i) == minutes) {
          MediBoxAlarm::ringAlarm(i);
          MediBoxAlarm::setAlarmTriggered(i, true);  // Use setter instead of direct access
        }
      }
    }
  }
}

void MediBoxTime::updateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  seconds = timeinfo.tm_sec;
  days = timeinfo.tm_mday;
  dayOfWeek = DAYS_OF_WEEK[timeinfo.tm_wday];
}

int MediBoxTime::getDays() { return days; }
String MediBoxTime::getDayOfWeek() { return dayOfWeek; }
int MediBoxTime::getHours() { return hours; }
int MediBoxTime::getMinutes() { return minutes; }
int MediBoxTime::getSeconds() { return seconds; }
int MediBoxTime::getUtcOffset() { return utcOffset; }
void MediBoxTime::setUtcOffset(int hours, int mins) {
  offsetHours = hours;
  offsetMins = mins;
  utcOffset = (hours < 0) ? (hours * 3600) - (mins * 60) : (hours * 3600) + (mins * 60);
  configTime(utcOffset, UTC_OFFSET_DST, NTP_SERVER);
}