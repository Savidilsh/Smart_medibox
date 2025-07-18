#include "MediBoxDisplay.h"
#include <Wire.h>

Adafruit_SSD1306 MediBoxDisplay::display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void MediBoxDisplay::initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextWrap(false);
  printLine("Welcome to", 0, 20, 2);
  printLine(" Medibox!", 10, 40, 2);
  delay(1000);
}

void MediBoxDisplay::displayTime(int days, String dayOfWeek, int hours, int minutes, int seconds,
                                 float temperature, float humidity, bool alarmEnabled) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(dayOfWeek);
  display.print(", ");
  display.print(days);

  display.setTextSize(3);
  display.setCursor(10, 16);
  char timeStr[10];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", hours, minutes);
  display.print(timeStr);

  display.setTextSize(1);
  display.setCursor(110, 35);
  snprintf(timeStr, sizeof(timeStr), "%02d", seconds);
  display.print(timeStr);

  bool tempWarning = (temperature < 24 || temperature > 32);
  bool humWarning = (humidity < 65 || humidity > 80);

  display.setCursor(0, 45);
  display.print("T:");
  display.print(temperature);
  if (tempWarning) display.print("!");
  display.print("C H:");
  display.print(humidity);
  if (humWarning) display.print("!");
  display.print("%");

  if (tempWarning || humWarning) {
    display.fillRect(0, 56, display.width(), 8, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 57);
    if (tempWarning && humWarning) display.print("Temp & Hum Warning!");
    else if (tempWarning) display.print("Temperature Warning!");
    else if (humWarning) display.print("Humidity Warning!");
  } else {
    display.fillRect(0, 56, display.width(), 8, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 57);
    display.print(alarmEnabled ? "ALARM ACTIVE" : "ALARM OFF");
  }

  display.display();
}

void MediBoxDisplay::printLine(String text, int column, int row, int textSize) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);
  display.display();
}