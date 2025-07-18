#include "MediBoxAlarm.h"
#include "MediBoxDisplay.h"
#include "MediBoxMenu.h"

bool MediBoxAlarm::alarmEnabled = false;
int MediBoxAlarm::alarmHours[N_ALARMS] = {0, 0, 0, 0};
int MediBoxAlarm::alarmMinutes[N_ALARMS] = {0, 0, 0, 0};
bool MediBoxAlarm::alarmTriggered[N_ALARMS] = {false, false, false, false};
bool MediBoxAlarm::alarmIsTemp[N_ALARMS] = {false, false, false, false};
int MediBoxAlarm::permanentAlarmCount = 2;
const int MediBoxAlarm::musicalNotes[N_NOTES] = {262, 294, 330, 349, 392, 440, 494, 523};

void MediBoxAlarm::initAlarm() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_3, OUTPUT);
  ledcSetup(BUZZER_CHANNEL, 2000, 8);
  ledcAttachPin(BUZZER, BUZZER_CHANNEL);
}

void MediBoxAlarm::ringAlarm(int alarmIndex) {
  digitalWrite(LED_3, HIGH);

  char alarmTimeStr[20];
  snprintf(alarmTimeStr, sizeof(alarmTimeStr), "%02d:%02d", 
           alarmHours[alarmIndex], alarmMinutes[alarmIndex]);

  MediBoxDisplay::display.clearDisplay();
  MediBoxDisplay::display.setTextSize(2);
  MediBoxDisplay::display.setCursor(0, 10);
  MediBoxDisplay::display.print(" Medicine Time:");
  MediBoxDisplay::display.setTextSize(1);
  MediBoxDisplay::display.setCursor(0, 50);
  MediBoxDisplay::display.print("OK:Snooze CANCEL:Off");
  MediBoxDisplay::display.display();

  delay(100);  
  bool alarmStopped = false;
  unsigned long toneStartTime = millis();
  int noteIndex = 0;
  const unsigned long TONE_INTERVAL = 502;  

  while (!alarmStopped) {
    unsigned long currentTime = millis();

    if (currentTime - toneStartTime >= TONE_INTERVAL) {
      noTone(BUZZER);
      noteIndex = (noteIndex + 1) % N_NOTES;
      tone(BUZZER, musicalNotes[noteIndex]);
      toneStartTime = currentTime;
    }

    if (digitalRead(PB_CANCEL) == LOW) {
      noTone(BUZZER);
      delay(200);
      alarmStopped = true;
      handleAlarmCancel(alarmIndex);
    } else if (digitalRead(PB_OK) == LOW) {
      noTone(BUZZER);
      delay(200);
      alarmStopped = true;
      handleAlarmSnooze(alarmIndex);
    }
  }

  digitalWrite(LED_3, LOW);
  MediBoxMenu::resetToHomeScreen();
}

void MediBoxAlarm::setAlarm(int alarmIndex) {
  if (alarmIndex >= permanentAlarmCount) return;
  alarmEnabled = true;
  int tempHour = alarmHours[alarmIndex];
  int tempMinute = alarmMinutes[alarmIndex];
  bool canceled = false;

  while (true) {
    MediBoxDisplay::display.clearDisplay();
    MediBoxDisplay::display.setTextSize(1);
    MediBoxDisplay::display.setCursor(0, 0);
    MediBoxDisplay::display.print("Set Alarm ");
    MediBoxDisplay::display.print(alarmIndex + 1);
    MediBoxDisplay::display.setTextSize(2);
    MediBoxDisplay::display.setCursor(20, 20);
    char alarmStr[10];
    snprintf(alarmStr, sizeof(alarmStr), "%02d:%02d", tempHour, tempMinute);
    MediBoxDisplay::display.print(alarmStr);
    MediBoxDisplay::display.display();

    int pressed = MediBoxMenu::waitForMenuButton();
    if (pressed == PB_UP) tempHour = (tempHour + 1) % 24;
    else if (pressed == PB_DOWN) tempHour = (tempHour - 1 + 24) % 24;
    else if (pressed == PB_OK) break;
    else if (pressed == PB_CANCEL) { canceled = true; break; }
  }

  if (!canceled) {
    while (true) {
      MediBoxDisplay::display.clearDisplay();
      MediBoxDisplay::display.setTextSize(1);
      MediBoxDisplay::display.setCursor(0, 0);
      MediBoxDisplay::display.print("Set Alarm ");
      MediBoxDisplay::display.print(alarmIndex + 1);
      MediBoxDisplay::display.setTextSize(2);
      MediBoxDisplay::display.setCursor(20, 20);
      char alarmStr[10];
      snprintf(alarmStr, sizeof(alarmStr), "%02d:%02d", tempHour, tempMinute);
      MediBoxDisplay::display.print(alarmStr);
      MediBoxDisplay::display.display();

      int pressed = MediBoxMenu::waitForMenuButton();
      if (pressed == PB_UP) tempMinute = (tempMinute + 1) % 60;
      else if (pressed == PB_DOWN) tempMinute = (tempMinute - 1 + 60) % 60;
      else if (pressed == PB_OK) {
        alarmHours[alarmIndex] = tempHour;
        alarmMinutes[alarmIndex] = tempMinute;
        alarmTriggered[alarmIndex] = false;
        alarmIsTemp[alarmIndex] = false;
        break;
      } else if (pressed == PB_CANCEL) { canceled = true; break; }
    }
  }

  if (!canceled) {
    MediBoxDisplay::display.clearDisplay();
    MediBoxDisplay::display.setTextSize(2);
    MediBoxDisplay::display.setCursor(10, 20);
    MediBoxDisplay::display.print("Alarm ");
    MediBoxDisplay::display.print(alarmIndex + 1);
    MediBoxDisplay::display.setCursor(10, 40);
    MediBoxDisplay::display.print("Set!");
    MediBoxDisplay::display.display();
    delay(1000);
  }
  MediBoxMenu::resetToHomeScreen();
}

void MediBoxAlarm::handleAlarmSnooze(int alarmIndex) {
  for (int i = permanentAlarmCount; i < N_ALARMS; i++) {
    if (alarmIsTemp[i]) {
      alarmHours[i] = 0;
      alarmMinutes[i] = 0;
      alarmTriggered[i] = false;
      alarmIsTemp[i] = false;
      break;
    }
  }

  for (int i = permanentAlarmCount; i < N_ALARMS; i++) {
    if (!alarmIsTemp[i] && alarmHours[i] == 0 && alarmMinutes[i] == 0) {
      int totalMinutes = alarmHours[alarmIndex] * 60 + alarmMinutes[alarmIndex] + SNOOZE_DURATION_MINUTES;
      alarmHours[i] = (totalMinutes / 60) % 24;
      alarmMinutes[i] = totalMinutes % 60;
      alarmTriggered[i] = false;
      alarmIsTemp[i] = true;
      alarmEnabled = true;

      MediBoxDisplay::display.clearDisplay();
      MediBoxDisplay::display.setTextSize(2);
      MediBoxDisplay::display.setCursor(10, 20);
      MediBoxDisplay::display.print("Snoozed ");
      MediBoxDisplay::display.print(SNOOZE_DURATION_MINUTES);
      MediBoxDisplay::display.print("m");
      MediBoxDisplay::display.display();
      delay(1000);
      break;
    }
  }
}

void MediBoxAlarm::handleAlarmCancel(int alarmIndex) {
  for (int i = permanentAlarmCount; i < N_ALARMS; i++) {
    if (alarmIsTemp[i]) {
      alarmHours[i] = 0;
      alarmMinutes[i] = 0;
      alarmTriggered[i] = false;
      alarmIsTemp[i] = false;
    }
  }

  MediBoxDisplay::display.clearDisplay();
  MediBoxDisplay::display.setTextSize(2);
  MediBoxDisplay::display.setCursor(10, 20);
  MediBoxDisplay::display.print("Alarm Off");
  MediBoxDisplay::display.display();
  delay(1000);
}

void MediBoxAlarm::disableAllAlarms() {
  alarmEnabled = false;
  for (int i = 0; i < N_ALARMS; i++) {
    alarmTriggered[i] = false;
  }
  MediBoxDisplay::display.clearDisplay();
  MediBoxDisplay::display.setTextSize(2);
  MediBoxDisplay::display.setCursor(10, 20);
  MediBoxDisplay::display.print("Alarms");
  MediBoxDisplay::display.setCursor(20, 40);
  MediBoxDisplay::display.print("Disabled");
  MediBoxDisplay::display.display();
  delay(1000);
  MediBoxMenu::resetToHomeScreen();
}

void MediBoxAlarm::viewAlarms() {
  MediBoxDisplay::display.clearDisplay();
  MediBoxDisplay::display.setTextSize(1);
  MediBoxDisplay::display.setCursor(0, 0);
  MediBoxDisplay::display.print("Active Alarms:");
  for (int i = 0; i < N_ALARMS; i++) {
    if (alarmHours[i] != 0 || alarmMinutes[i] != 0) {
      MediBoxDisplay::display.setCursor(0, 10 + i * 10);
      MediBoxDisplay::display.print("Alarm ");
      MediBoxDisplay::display.print(i + 1);
      MediBoxDisplay::display.print(": ");
      char alarmStr[6];
      snprintf(alarmStr, sizeof(alarmStr), "%02d:%02d", alarmHours[i], alarmMinutes[i]);
      MediBoxDisplay::display.print(alarmStr);
      MediBoxDisplay::display.print(alarmIsTemp[i] ? " (TEMP)" : " (ON)");
    }
  }
  MediBoxDisplay::display.display();
  MediBoxMenu::waitForMenuButton();
  MediBoxMenu::resetToHomeScreen();
}

void MediBoxAlarm::deleteAlarm() {
  int selectedAlarm = 0;
  while (true) {
    MediBoxDisplay::display.clearDisplay();
    MediBoxDisplay::display.setTextSize(1);
    MediBoxDisplay::display.setCursor(0, 0);
    MediBoxDisplay::display.print("Delete Alarm ");
    MediBoxDisplay::display.print(selectedAlarm + 1);
    MediBoxDisplay::display.display();

    int pressed = MediBoxMenu::waitForMenuButton();
    if (pressed == PB_UP) selectedAlarm = (selectedAlarm + 1) % permanentAlarmCount;
    else if (pressed == PB_DOWN) selectedAlarm = (selectedAlarm - 1 + permanentAlarmCount) % permanentAlarmCount;
    else if (pressed == PB_OK) {
      alarmHours[selectedAlarm] = 0;
      alarmMinutes[selectedAlarm] = 0;
      alarmTriggered[selectedAlarm] = false;
      alarmIsTemp[selectedAlarm] = false;
      MediBoxDisplay::display.clearDisplay();
      MediBoxDisplay::display.setTextSize(2);
      MediBoxDisplay::display.setCursor(10, 20);
      MediBoxDisplay::display.print("Alarm ");
      MediBoxDisplay::display.print(selectedAlarm + 1);
      MediBoxDisplay::display.print(" Deleted");
      MediBoxDisplay::display.display();
      delay(1000);
      break;
    } else if (pressed == PB_CANCEL) break;
  }
  MediBoxMenu::resetToHomeScreen();
}

bool MediBoxAlarm::isAlarmEnabled() { return alarmEnabled; }
int MediBoxAlarm::getAlarmHours(int index) { return alarmHours[index]; }
int MediBoxAlarm::getAlarmMinutes(int index) { return alarmMinutes[index]; }
bool MediBoxAlarm::isAlarmTriggered(int index) { return alarmTriggered[index]; }
void MediBoxAlarm::setAlarmTriggered(int index, bool triggered) {
  if (index >= 0 && index < N_ALARMS) {
    alarmTriggered[index] = triggered;
  }
}