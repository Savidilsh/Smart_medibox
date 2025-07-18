#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include "MediBoxDisplay.h"
#include "MediBoxAlarm.h"
#include "MediBoxTime.h"
#include "MediBoxSensor.h"
#include "MediBoxMenu.h"
#include "MediBoxConfig.h"

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

Servo servo;

MediBoxConfig config;

unsigned long last_sample_time = 0;
unsigned long last_send_time = 0;

float ldr_sum = 0.0;
int ldr_count = 0;
float last_ldr_avg = 0.0; 

float temperature = 0.0;
float humidity = 0.0;
float servo_angle = 0.0;

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT");
      client.subscribe("medibox/cfg/ts");
      client.subscribe("medibox/cfg/tu");
      client.subscribe("medibox/cfg/servo");
      Serial.println("Subscribed to configuration topics");
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(client.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Received on topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (strcmp(topic, "medibox/cfg/ts") == 0) {
    float value = message.toFloat();
    if (value > 0) {
      config.setSamplingInterval(value);
      Serial.printf("Updated ts: %.2f s\n", value);
    }
  } else if (strcmp(topic, "medibox/cfg/tu") == 0) {
    float value = message.toFloat();
    if (value > 0) {
      config.setSendingInterval(value);
      Serial.printf("Updated tu: %.2f s\n", value);
    }
  } else if (strcmp(topic, "medibox/cfg/servo") == 0) {
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, message);
    if (!error) {
      if (doc.containsKey("theta")) {
        config.setThetaOffset(doc["theta"].as<float>());
      }
      if (doc.containsKey("gamma")) {
        config.setGamma(doc["gamma"].as<float>());
      }
      if (doc.containsKey("Tmed")) {
        config.setTMed(doc["Tmed"].as<float>());
      }
      Serial.println("Updated servo parameters");
    } else {
      Serial.print("Failed to parse servo config: ");
      Serial.println(error.c_str());
    }
  }
}

void setup() {
  Serial.begin(9600);

  MediBoxDisplay::initDisplay();
  MediBoxTime::initTime();
  MediBoxSensor::initSensor();
  MediBoxAlarm::initAlarm();
  MediBoxMenu::initMenu();
  config.begin();

  config.setSamplingInterval(1.0); 
  config.setSendingInterval(1.0);  
  servo.attach(18, 500, 2400); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();

  last_sample_time = 0;
  last_send_time = 0;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long current_time = millis();

  if (current_time - last_sample_time >= (unsigned long)(config.getSamplingInterval() * 1000)) {
    MediBoxSensor::checkTempAndHumidity();
    temperature = MediBoxSensor::getTemperature();
    humidity = MediBoxSensor::getHumidity();
    float ldr_value = MediBoxSensor::getLightIntensity();
    ldr_sum += ldr_value;
    ldr_count++;

    Serial.print("Sampled at ");
    Serial.print(current_time / 1000);
    Serial.print("s: ldr_value=");
    Serial.println(ldr_value);

    if (ldr_count > 0) {
      last_ldr_avg = ldr_sum / ldr_count;
    }

    float theta_offset = config.getThetaOffset();
    float gamma = config.getGamma();
    float T_med = config.getTMed();
    float t_s = config.getSamplingInterval();
    float t_u = config.getSendingInterval();
    float I = ldr_value; 
    float T = temperature;

    if (t_u > 0 && t_s > 0 && T_med > 0) {
      float log_term = log(t_s / t_u);
      float theta = theta_offset + (180 - theta_offset) * I * gamma * log_term * (T / T_med);
      theta = constrain(theta, 0, 180); 
      servo_angle = theta;
      int servo_command = (int)servo_angle;
      servo.write(servo_command);
      Serial.print("Servo angle set to: ");
      Serial.print(servo_angle);
      Serial.print(" (command: ");
      Serial.print(servo_command);
      Serial.println(")");
    } else {
      Serial.println("Invalid parameters for servo calculation");
    }

    MediBoxDisplay::displayTime(
      MediBoxTime::getDays(), MediBoxTime::getDayOfWeek(),
      MediBoxTime::getHours(), MediBoxTime::getMinutes(),
      MediBoxTime::getSeconds(), temperature, humidity,
      MediBoxAlarm::isAlarmEnabled()
    );

    last_sample_time = current_time;
  }

  if (current_time - last_send_time >= (unsigned long)(config.getSendingInterval() * 1000)) {
    Serial.print("Attempting to send data at ");
    Serial.print(current_time / 1000);
    Serial.println(" seconds");

    float ldr_avg = (ldr_count > 0) ? (ldr_sum / ldr_count) : last_ldr_avg; 

    uint8_t buffer[12];
    memcpy(buffer, &ldr_avg, 4);      
    memcpy(buffer + 4, &temperature, 4); 
    memcpy(buffer + 8, &servo_angle, 4); 

    if (client.publish("medibox/data", buffer, 12)) {
      Serial.println("Data published successfully");
      Serial.print("Published: ldr_avg="); Serial.print(ldr_avg);
      Serial.print(", temperature="); Serial.print(temperature);
      Serial.print(", servo_angle="); Serial.println(servo_angle);
    } else {
      Serial.println("Data publish failed");
    }

    ldr_sum = 0.0;
    ldr_count = 0;
    last_send_time = current_time;
  }

  MediBoxTime::updateTimeWithCheckAlarm();
  MediBoxMenu::handleButtonPresses();
}