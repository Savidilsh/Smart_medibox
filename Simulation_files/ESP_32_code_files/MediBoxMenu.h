#ifndef MEDIBOX_MENU_H
#define MEDIBOX_MENU_H

#include <Arduino.h>
#include "MediBoxConfig.h"

enum MenuState {
  HOME_SCREEN,
  MAIN_MENU,
  TIME_ZONE_SETTING,
  ALARM_SETTING
};

class MediBoxMenu {
public:
  static void initMenu();
  static void handleButtonPresses();
  static void displayMenu();
  static int waitForMenuButton();
  static void setTimeZone();
  static void resetToHomeScreen();
  static MenuState getCurrentState();

private:
  static MenuState currentState;
  static int currentMenuIndex;
  static int menuScrollOffset;
  static const String MENU_ITEMS[MAX_VISIBLE_MENU_ITEMS + 3];
  static void handleOkButton();
  static void handleCancelButton();
  static void playButtonSound();
};

#endif