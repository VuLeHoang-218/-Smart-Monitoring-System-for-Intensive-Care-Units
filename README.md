# Smart-Monitoring-System-for-Intensive-Care-Units
# Patient Monitoring System using ESP32

## Overview

This project aims to create a patient monitoring system for hospitals using ESP32 microcontrollers. The system consists of a gateway ESP32 and two ESP32 units equipped with sensors to measure body temperature and BPM (heart rate). The data collected from the sensors is sent to the gateway, which processes and transmits it for monitoring and analysis.

---

## Features

- **Real-time patient monitoring**: Tracks body temperature and BPM.
- **Wireless communication**: Uses ESP32's Wi-Fi capabilities for data transmission.
- **Centralized gateway**: Aggregates data from multiple ESP32 sensor units.
- **Scalable design**: Supports monitoring of multiple patients.

---

## Components

### Hardware:

- 3 ESP32 microcontrollers (1 for the gateway and 2 for sensors)
- Temperature sensor (e.g., DHT11, DS18B20)
- BPM sensor (e.g., MAX30100, MAX30102)
- Power supply (e.g., batteries or USB cables)

### Software:

- Arduino IDE
- Required libraries:
  - **WiFi**: For ESP32 Wi-Fi communication
  - **DHT**: For temperature sensor data processing (if using DHT11/22)
  - **Adafruit\_Sensor**: For sensor abstraction (optional)
  - **MAX30100**: For BPM sensor (if applicable)

---

## System Architecture

1. **Sensor Units**:

   - Each ESP32 reads data from the connected temperature and BPM sensors.
   - Sends the data wirelessly to the gateway ESP32 using Wi-Fi.

2. **Gateway ESP32**:

   - Receives sensor data from the sensor units.
   - Processes the data and forwards it to a central monitoring system (e.g., PC, cloud server, or mobile app).

---

## Installation and Setup

### Step 1: Prepare the ESP32s

1. Install the **Arduino IDE** on your computer.
2. Add the ESP32 board to the Arduino IDE:
   - Open `Preferences` and add the ESP32 URL to the "Additional Boards Manager URLs":
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Go to `Tools > Board > Boards Manager`, search for "ESP32," and install.

### Step 2: Install Required Libraries

- Install the necessary libraries through the Arduino Library Manager (`Tools > Manage Libraries`).
  - Examples: `DHT Sensor Library`, `Adafruit Unified Sensor`, `WiFi`, `MAX30100` (if using).

### Step 3: Upload Code

1. Open the provided code for the gateway ESP32 and upload it to the first ESP32.
2. Open the provided code for the sensor ESP32 units and upload them to the other two ESP32s.
3. Make sure to adjust Wi-Fi credentials and any required configurations in the code.

### Step 4: Connect Sensors

1. Connect the temperature sensor and BPM sensor to the respective ESP32 sensor units.
2. Verify the wiring matches the pin configurations defined in the code.

---

## Usage

1. Power on all ESP32 devices.
2. Ensure the ESP32 devices are connected to the same Wi-Fi network.
3. The sensor units will begin collecting data and transmitting it to the gateway ESP32.
4. Monitor the data from the gateway ESP32 output (e.g., serial monitor, server, or mobile app).


## Future Improvements

- Add a mobile application or web interface for real-time data visualization.
- Integrate cloud storage for long-term data analysis.
- Add support for more sensors (e.g., SpO2, ECG).

---

## License

This project is licensed under the MIT License. Feel free to modify and use it as needed.

