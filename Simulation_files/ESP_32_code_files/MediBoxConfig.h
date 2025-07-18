#ifndef MEDIBOX_CONFIG_H
#define MEDIBOX_CONFIG_H

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define BUZZER 5
#define LED_1 15  // Red: Temperature warning
#define LED_2 16  // Blue: Humidity warning
#define LED_3 17  // Yellow: Alarm ringing
#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 12
#define BUZZER_CHANNEL 0

#define N_ALARMS 4
#define N_NOTES 8
#define SNOOZE_DURATION_MINUTES 5
#define MAX_VISIBLE_MENU_ITEMS 3

#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET_DST 0

class MediBoxConfig {
public:
  MediBoxConfig();
  void begin();
  
  float getSamplingInterval();
  float getSendingInterval();
  float getThetaOffset();
  float getGamma();
  float getTMed();
  
  void setSamplingInterval(float ts);
  void setSendingInterval(float tu);
  void setThetaOffset(float theta);
  void setGamma(float g);
  void setTMed(float tmed);

private:
  float sampling_interval;
  float sending_interval;
  float theta_offset;
  float gamma;
  float T_med;
};

#endif