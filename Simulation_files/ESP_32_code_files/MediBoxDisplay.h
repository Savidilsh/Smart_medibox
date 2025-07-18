#ifndef MEDIBOX_DISPLAY_H
#define MEDIBOX_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MediBoxConfig.h"

class MediBoxDisplay {
public:
  static Adafruit_SSD1306 display;  
  static void initDisplay();
  static void displayTime(int days, String dayOfWeek, int hours, int minutes, int seconds,
                          float temperature, float humidity, bool alarmEnabled);
  static void printLine(String text, int column, int row, int textSize);
};

#endif