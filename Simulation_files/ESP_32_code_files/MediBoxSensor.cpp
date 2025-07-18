#include "MediBoxSensor.h"

DHTesp MediBoxSensor::dhtSensor;
unsigned long MediBoxSensor::lastTempWarningTime = 0;
unsigned long MediBoxSensor::lastHumWarningTime = 0;
const unsigned long MediBoxSensor::WARNING_INTERVAL = 1000;
const int MediBoxSensor::musicalNotes[N_NOTES] = {262, 294, 330, 349, 392, 440, 494, 523};
const int MediBoxSensor::LDR_PIN = 36; // Define LDR pin

void MediBoxSensor::initSensor() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  dhtSensor.setup(DHTPIN, DHTesp::DHT22);
}

void MediBoxSensor::checkTempAndHumidity() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool tempWarning = (data.temperature < 24 || data.temperature > 32);
  bool humWarning = (data.humidity < 65 || data.humidity > 80);

  unsigned long currentTime = millis();

  if (tempWarning) {
    if (currentTime - lastTempWarningTime >= WARNING_INTERVAL) {
      digitalWrite(LED_1, !digitalRead(LED_1));
      tone(BUZZER, musicalNotes[0], 100);
      lastTempWarningTime = currentTime;
    }
  } else {
    digitalWrite(LED_1, LOW);
  }

  if (humWarning) {
    if (currentTime - lastHumWarningTime >= WARNING_INTERVAL) {
      digitalWrite(LED_2, !digitalRead(LED_2));
      tone(BUZZER, musicalNotes[2], 100);
      lastHumWarningTime = currentTime;
    }
  } else {
    digitalWrite(LED_2, LOW);
  }
}

float MediBoxSensor::getTemperature() {
  return dhtSensor.getTempAndHumidity().temperature;
}

float MediBoxSensor::getHumidity() {
  return dhtSensor.getTempAndHumidity().humidity;
}

float MediBoxSensor::getLightIntensity() {
  int ldrRaw = analogRead(LDR_PIN); // 0-4095 for ESP32
  return (1-(ldrRaw / 4095.0)); // Normalize to 0-1
}