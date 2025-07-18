#ifndef MEDIBOX_SENSOR_H
#define MEDIBOX_SENSOR_H

#include <Arduino.h>
#include <DHTesp.h>
#include "MediBoxConfig.h"

class MediBoxSensor {
public:
  static void initSensor();
  static void checkTempAndHumidity();
  static float getTemperature();
  static float getHumidity();
  static float getLightIntensity();

private:
  static DHTesp dhtSensor;
  static unsigned long lastTempWarningTime;
  static unsigned long lastHumWarningTime;
  static const unsigned long WARNING_INTERVAL;
  static const int musicalNotes[N_NOTES];
  static const int LDR_PIN;
};

#endif