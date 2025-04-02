# Explo
# Swarm Bot Control System

This project consists of an ESP32-based swarm bot firmware and a Python-based GUI for controlling the swarm formation. The ESP32 firmware handles motor control, sensor readings, and UDP communication, while the GUI provides an interface to send formation commands.

## Features
- Control bots via a Python GUI
- Supported formations: Straight Line, Triangle, Circle (with configurable radius)
- ESP32 firmware for motor and sensor control
- UDP communication for real-time command execution

## Installation and Setup

### ESP32 Firmware
1. Install the **Arduino IDE** and add ESP32 board support.
2. Install the required libraries:
   - `WiFi.h`
   - `WiFiUdp.h`
   - `Wire.h`
   - `Adafruit_MPU6050.h`
   - `Adafruit_Sensor.h`
3. Connect the ESP32 to your computer and upload the firmware from `esp32_swarm_bot.ino`.
4. Ensure the ESP32 connects to Wi-Fi and listens for UDP commands on port `4210`.

### Python GUI
1. Install Python (if not installed already).
2. Install Tkinter (usually pre-installed with Python):
   ```bash
   pip install tk
### Usage
   - `Open the GUI and select a formation.`
   - `If choosing a circle formation, enter a radius before clicking the button.`
   - `The ESP32 bots will execute the selected formation.`