# Smart MediBox 💊⏰

An intelligent medicine reminder system built with ESP32, featuring environmental monitoring, IoT connectivity, and automated dispensing capabilities.

![MediBox](https://img.shields.io/badge/Platform-ESP32-blue)
![Language](https://img.shields.io/badge/Language-C++-green)
![Framework](https://img.shields.io/badge/Framework-Arduino-orange)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-red)

## 🌟 Features

### 📋 Core Functionality
- **Multiple Alarm System**: Up to 4 configurable medicine alarms with snooze functionality
- **Real-time Clock**: NTP-synchronized time with timezone support
- **Environmental Monitoring**: Temperature, humidity, and light intensity sensing
- **Smart Dispensing**: Servo-controlled mechanism based on environmental conditions
- **Visual Feedback**: 128x64 OLED display with intuitive menu system
- **Audio Alerts**: Musical tone sequences for alarms and warnings

### 🌐 IoT Capabilities
- **WiFi Connectivity**: Remote monitoring and configuration
- **MQTT Integration**: Real-time data publishing and parameter updates
- **Remote Configuration**: Adjust sampling intervals, servo parameters, and more
- **Data Logging**: Continuous sensor data transmission

### 🎛️ User Interface
- **Button Navigation**: 4-button interface (UP, DOWN, OK, CANCEL)
- **Menu System**: Easy-to-navigate settings and alarm configuration
- **Status LEDs**: Visual indicators for temperature, humidity, and alarm states
- **Warning System**: Real-time alerts for environmental conditions

## 🔧 Hardware Components

### Main Controller
- **ESP32 DevKit C V4**: Main microcontroller with WiFi capability

### Sensors & Actuators
- **DHT22**: Temperature and humidity sensor
- **LDR (Photoresistor)**: Light intensity measurement
- **SG90 Servo Motor**: Medicine dispensing mechanism
- **SSD1306 OLED Display**: 128x64 pixel display for UI

### User Interface
- **4 Push Buttons**: Navigation and control
- **3 Status LEDs**: 
  - Red: Temperature warning
  - Blue: Humidity warning  
  - Yellow: Alarm indicator
- **Buzzer**: Audio feedback and alarms

### Circuit Protection
- **Pull-up Resistors**: Button debouncing and stable inputs
- **Current Limiting Resistors**: LED protection

## 📦 Installation

### Prerequisites
- [PlatformIO](https://platformio.org/) IDE or extension
- ESP32 development environment

### Dependencies
The following libraries are automatically managed by PlatformIO:

```ini
lib_deps = 
    adafruit/Adafruit GFX Library@^1.12.0
    beegee-tokyo/DHT sensor library for ESPx@^1.19
    adafruit/Adafruit SSD1306@^2.5.13
    knolleary/PubSubClient@^2.8
    madhephaestus/ESP32Servo@^0.11.0
    bblanchon/ArduinoJson @ ^6.21.5
```

### Setup Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/smart-medibox.git
   cd smart-medibox
   ```

2. **Hardware Assembly**
   - Follow the circuit diagram in `diagram.json`
   - Connect all components according to pin definitions in `MediBoxConfig.h`

3. **Software Configuration**
   - Update WiFi credentials in `main.cpp`
   - Configure MQTT broker settings if needed
   - Adjust pin assignments in `MediBoxConfig.h` if necessary

4. **Build and Upload**
   ```bash
   pio run --target upload
   ```

## 📊 Circuit Diagram

### Pin Configuration
```cpp
#define BUZZER 5
#define LED_1 15    // Red: Temperature warning
#define LED_2 16    // Blue: Humidity warning  
#define LED_3 17    // Yellow: Alarm ringing
#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 12
#define LDR_PIN 36
#define SERVO_PIN 18
```

### I2C Connections
- **SDA**: GPIO 21
- **SCL**: GPIO 22
- **OLED Address**: 0x3C

## 🎮 Usage

### First Setup
1. Power on the device
2. Connect to "Wokwi-GUEST" WiFi network
3. Set your timezone through the menu system
4. Configure your medicine alarms

### Setting Alarms
1. Press **OK** to enter main menu
2. Navigate to "Set Alarm 1" or "Set Alarm 2"
3. Use **UP/DOWN** to adjust hours and minutes
4. Press **OK** to confirm each setting

### Menu Navigation
- **OK**: Enter menu / Confirm selection
- **CANCEL**: Go back / Cancel operation
- **UP/DOWN**: Navigate options / Adjust values

### Alarm Functions
- **OK during alarm**: Snooze for 5 minutes
- **CANCEL during alarm**: Turn off alarm

## 🌐 MQTT Integration

### Published Topics
- `medibox/data`: Binary sensor data (12 bytes)
  - Light intensity (4 bytes, float)
  - Temperature (4 bytes, float)
  - Servo angle (4 bytes, float)

### Subscribed Topics
- `medibox/cfg/ts`: Sampling interval configuration
- `medibox/cfg/tu`: Sending interval configuration  
- `medibox/cfg/servo`: Servo parameters (JSON format)

### MQTT Broker
- **Default**: test.mosquitto.org:1883
- **Client ID**: ESP32Client-[random]

## 🤖 Smart Dispensing Algorithm

The servo motor angle is calculated using environmental factors:

```cpp
θ = θ₀ + (180 - θ₀) × I × γ × ln(ts/tu) × (T/Tmed)
```

Where:
- `θ₀`: Base offset angle
- `I`: Light intensity (0-1)
- `γ`: Gamma correction factor
- `ts`: Sampling interval
- `tu`: Sending interval
- `T`: Current temperature
- `Tmed`: Reference temperature

## 📈 Environmental Monitoring

### Temperature Alerts
- **Normal Range**: 24°C - 32°C
- **Warning**: Red LED flashes + audio alert

### Humidity Alerts  
- **Normal Range**: 65% - 80%
- **Warning**: Blue LED flashes + audio alert

### Light Monitoring
- Continuous LDR readings
- Influences dispensing mechanism
- Data logged via MQTT

## 🔧 Configuration

### Default Parameters
```cpp
// Timing
sampling_interval = 5.0    // seconds
sending_interval = 120.0   // seconds

// Servo Control
theta_offset = 30.0        // degrees
gamma = 0.75              // correction factor
T_med = 30.0              // reference temperature (°C)

// Alarms
SNOOZE_DURATION = 5       // minutes
MAX_ALARMS = 4            // total alarms
```

### Remote Configuration
Parameters can be updated via MQTT:

```json
// Servo configuration
{
  "theta": 45.0,
  "gamma": 0.8,
  "Tmed": 25.0
}
```

## 🏗️ Project Structure

```
smart-medibox/
├── src/
│   ├── main.cpp              # Main application logic
│   ├── MediBoxAlarm.cpp/h    # Alarm management
│   ├── MediBoxConfig.cpp/h   # Configuration settings
│   ├── MediBoxDisplay.cpp/h  # OLED display control
│   ├── MediBoxMenu.cpp/h     # Menu system
│   ├── MediBoxSensor.cpp/h   # Sensor readings
│   └── MediBoxTime.cpp/h     # Time management
├── platformio.ini            # PlatformIO configuration
├── diagram.json             # Wokwi circuit diagram
├── flows.json              # Node-RED MQTT flows
└── README.md               # This file
```

## 🚀 Future Enhancements

- [ ] Web-based configuration interface
- [ ] Mobile app integration
- [ ] Multi-user support with individual profiles
- [ ] Prescription schedule integration
- [ ] Voice command support
- [ ] Battery backup system
- [ ] Dose counting and inventory tracking

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**Savindu Wickramasinghe**
- Electronics and Telecommunication Engineering Undergraduate
- University of Moratuwa, Sri Lanka
- 📧 savindudilshan124@gmail.com
- 🔗 [LinkedIn](https://linkedin.com/in/savindu-wickramasinghe)
- 💻 [GitHub](https://github.com/savidilsh)

## 🙏 Acknowledgments

- University of Moratuwa Electronics and Telecommunication Engineering Department
- Wokwi simulator for circuit design and testing
- Open source community for excellent libraries
- Node-RED community for MQTT integration examples

## 📞 Support

If you have any questions or need help with the project, feel free to:
- Open an issue on GitHub
- Contact me via email
- Check the [Wiki](../../wiki) for detailed documentation

---

⭐ If you found this project helpful, please give it a star!
