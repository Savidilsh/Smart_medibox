#include "MediBoxConfig.h"

MediBoxConfig::MediBoxConfig() {
  sampling_interval = 5.0; // Default 5 seconds
  sending_interval = 120.0; // Default 2 minutes
  theta_offset = 30.0; // Default 30 degrees
  gamma = 0.75; // Default 0.75
  T_med = 30.0; // Default 30°C
}

void MediBoxConfig::begin() {
  // Add EEPROM or other storage initialization if needed
}

float MediBoxConfig::getSamplingInterval() { return sampling_interval; }
float MediBoxConfig::getSendingInterval() { return sending_interval; }
float MediBoxConfig::getThetaOffset() { return theta_offset; }
float MediBoxConfig::getGamma() { return gamma; }
float MediBoxConfig::getTMed() { return T_med; }

void MediBoxConfig::setSamplingInterval(float ts) { sampling_interval = ts; }
void MediBoxConfig::setSendingInterval(float tu) { sending_interval = tu; }
void MediBoxConfig::setThetaOffset(float theta) { theta_offset = theta; }
void MediBoxConfig::setGamma(float g) { gamma = g; }
void MediBoxConfig::setTMed(float tmed) { T_med = tmed; }