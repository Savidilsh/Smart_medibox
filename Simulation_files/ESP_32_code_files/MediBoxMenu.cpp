#include "MediBoxMenu.h"
#include "MediBoxDisplay.h"
#include "MediBoxAlarm.h"
#include "MediBoxTime.h"
#include "MediBoxSensor.h"

MenuState MediBoxMenu::currentState = HOME_SCREEN;
int MediBoxMenu::currentMenuIndex = 0;
int MediBoxMenu::menuScrollOffset = 0;
const String MediBoxMenu::MENU_ITEMS[MAX_VISIBLE_MENU_ITEMS + 3] = {
  "Set Time Zone", "View Alarms", "Set Alarm 1", "Set Alarm 2", "Delete Alarm", "Disable Alarms"
};

void MediBoxMenu::initMenu() {
  pinMode(PB_CANCEL, INPUT_PULLUP);
  pinMode(PB_OK, INPUT_PULLUP);
  pinMode(PB_UP, INPUT_PULLUP);
  pinMode(PB_DOWN, INPUT_PULLUP);
}

void MediBoxMenu::handleButtonPresses() {
  if (digitalRead(PB_OK) == LOW) {
    delay(200);
    playButtonSound();
    handleOkButton();
  }
  if (digitalRead(PB_CANCEL) == LOW) {
    delay(200);
    playButtonSound();
    handleCancelButton();
  }
  if (digitalRead(PB_UP) == LOW) {
    delay(200);
    playButtonSound();
  }
  if (digitalRead(PB_DOWN) == LOW) {
    delay(200);
    playButtonSound();
  }
}

void MediBoxMenu::displayMenu() {
  int totalMenuItems = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
  int visibleItems = min(MAX_VISIBLE_MENU_ITEMS, totalMenuItems);

  MediBoxDisplay::display.clearDisplay();
  MediBoxDisplay::display.setTextSize(1);

  for (int i = 0; i < visibleItems; i++) {
    int itemIndex = i + menuScrollOffset;
    if (itemIndex >= totalMenuItems) break;

    if (itemIndex == currentMenuIndex) {
      MediBoxDisplay::display.fillRect(0, i * 20, SCREEN_WIDTH, 20, SSD1306_WHITE);
      MediBoxDisplay::display.setTextColor(SSD1306_BLACK);
    } else {
      MediBoxDisplay::display.setTextColor(SSD1306_WHITE);
    }

    int16_t x1, y1;
    uint16_t w, h;
    MediBoxDisplay::display.getTextBounds(MENU_ITEMS[itemIndex], 0, 0, &x1, &y1, &w, &h);
    int16_t x = (SCREEN_WIDTH - w) / 2;
    MediBoxDisplay::display.setCursor(x, i * 20 + 10);
    MediBoxDisplay::display.println(MENU_ITEMS[itemIndex]);
    MediBoxDisplay::display.setTextColor(SSD1306_WHITE);
  }

  MediBoxDisplay::display.display();

  int pressed = waitForMenuButton();
  if (pressed == PB_UP) {
    if (currentMenuIndex == 0) currentMenuIndex = totalMenuItems - 1;
    else currentMenuIndex--;
    if (currentMenuIndex < menuScrollOffset) menuScrollOffset = currentMenuIndex;
    else if (currentMenuIndex >= menuScrollOffset + visibleItems) menuScrollOffset = currentMenuIndex - visibleItems + 1;
    displayMenu();
  } else if (pressed == PB_DOWN) {
    if (currentMenuIndex == totalMenuItems - 1) currentMenuIndex = 0;
    else currentMenuIndex++;
    if (currentMenuIndex < menuScrollOffset) menuScrollOffset = currentMenuIndex;
    else if (currentMenuIndex >= menuScrollOffset + visibleItems) menuScrollOffset = currentMenuIndex - visibleItems + 1;
    displayMenu();
  } else if (pressed == PB_OK) handleOkButton();
  else if (pressed == PB_CANCEL) resetToHomeScreen();
}

int MediBoxMenu::waitForMenuButton() {
  while (true) {
    if (digitalRead(PB_UP) == LOW) {
      delay(200);
      playButtonSound();
      return PB_UP;
    } else if (digitalRead(PB_DOWN) == LOW) {
      delay(200);
      playButtonSound();
      return PB_DOWN;
    } else if (digitalRead(PB_OK) == LOW) {
      delay(200);
      playButtonSound();
      return PB_OK;
    } else if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      playButtonSound();
      return PB_CANCEL;
    }
  }
}

void MediBoxMenu::setTimeZone() {
  int tempOffsetHour = MediBoxTime::getUtcOffset() / 3600;  // Approximate initial hour
  int tempOffsetMin = (MediBoxTime::getUtcOffset() % 3600) / 60;
  bool canceled = false;

  while (true) {
    MediBoxDisplay::display.clearDisplay();
    MediBoxDisplay::display.setTextSize(1);
    MediBoxDisplay::display.setCursor(0, 0);
    MediBoxDisplay::display.print("Set Time Zone (Hour)");
    MediBoxDisplay::display.setTextSize(2);
    MediBoxDisplay::display.setCursor(20, 20);
    MediBoxDisplay::display.print(tempOffsetHour);
    MediBoxDisplay::display.print(" hrs");
    MediBoxDisplay::display.display();

    int pressed = waitForMenuButton();
    if (pressed == PB_UP) tempOffsetHour = (tempOffsetHour + 1 > 14) ? -12 : tempOffsetHour + 1;
    else if (pressed == PB_DOWN) tempOffsetHour = (tempOffsetHour - 1 < -12) ? 14 : tempOffsetHour - 1;
    else if (pressed == PB_OK) break;
    else if (pressed == PB_CANCEL) { canceled = true; break; }
  }

  if (!canceled) {
    while (true) {
      MediBoxDisplay::display.clearDisplay();
      MediBoxDisplay::display.setTextSize(1);
      MediBoxDisplay::display.setCursor(0, 0);
      MediBoxDisplay::display.print("Set Time Zone (Mins)");
      MediBoxDisplay::display.setTextSize(2);
      MediBoxDisplay::display.setCursor(10, 20);
      MediBoxDisplay::display.print(tempOffsetMin);
      MediBoxDisplay::display.print(" min");
      MediBoxDisplay::display.display();

      int pressed = waitForMenuButton();
      if (pressed == PB_UP) tempOffsetMin = (tempOffsetMin + 5) % 60;
      else if (pressed == PB_DOWN) tempOffsetMin = (tempOffsetMin + 60 - 5) % 60;
      else if (pressed == PB_OK) {
        MediBoxTime::setUtcOffset(tempOffsetHour, tempOffsetMin);
        break;
      } else if (pressed == PB_CANCEL) { canceled = true; break; }
    }
  }

  if (!canceled) {
    MediBoxDisplay::display.clearDisplay();
    MediBoxDisplay::display.setTextSize(2);
    MediBoxDisplay::display.setCursor(10, 20);
    MediBoxDisplay::display.print("TZ Updated");
    MediBoxDisplay::display.display();
    delay(1000);
  }
  resetToHomeScreen();
}

void MediBoxMenu::resetToHomeScreen() {
  currentState = HOME_SCREEN;
  currentMenuIndex = 0;
  menuScrollOffset = 0;
  MediBoxDisplay::displayTime(MediBoxTime::getDays(), MediBoxTime::getDayOfWeek(), MediBoxTime::getHours(),
                              MediBoxTime::getMinutes(), MediBoxTime::getSeconds(),
                              MediBoxSensor::getTemperature(), MediBoxSensor::getHumidity(),
                              MediBoxAlarm::isAlarmEnabled());
}

MenuState MediBoxMenu::getCurrentState() { return currentState; }

void MediBoxMenu::handleOkButton() {
  switch (currentState) {
    case HOME_SCREEN:
      currentState = MAIN_MENU;
      displayMenu();
      break;
    case MAIN_MENU:
      switch (currentMenuIndex) {
        case 0: // Set Time Zone
          currentState = TIME_ZONE_SETTING;
          setTimeZone();
          break;
        case 1: // View Alarms
          MediBoxAlarm::viewAlarms();
          break;
        case 2: // Set Alarm 1
        case 3: // Set Alarm 2
          currentState = ALARM_SETTING;
          MediBoxAlarm::setAlarm(currentMenuIndex - 2);
          break;
        case 4: // Delete Alarm
          MediBoxAlarm::deleteAlarm();
          break;
        case 5: // Disable Alarms
          MediBoxAlarm::disableAllAlarms();
          break;
      }
      break;
  }
}

void MediBoxMenu::handleCancelButton() {
  resetToHomeScreen();
}

void MediBoxMenu::playButtonSound() {
  tone(BUZZER, 523, 100);
}