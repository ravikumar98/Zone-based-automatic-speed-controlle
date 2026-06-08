# Zone-Based Automatic Speed Controller 🚗🛡️

An IoT-integrated embedded automotive safety platform designed to automatically govern public vehicle maximum velocities within sensitive geographic locations (e.g., schools, hospitals) through real-time geofenced coordinate calculation.

## 📌 Project Architecture
Operator failure to obey regional speed limitations represents a core component of vehicle accident rates. This system interfaces directly with the vehicle's electronic drivetrain controller to throttle velocity parameters autonomously based on precise physical coordinates, overriding operator acceleration requests when crossing a geofenced safety zone boundary.

## 📊 Core System Layouts

### Hardware Module Block Diagram

```mermaid
graph TD
    Battery[Lithium-Ion Battery Pack] -->|Power Delivery| ESP32[ESP32 Microcontroller Core MCU]
    Battery -->|VCC Power Rail| Driver[L298N H-Bridge Motor Driver]
    GPS[Ublox Neo 6M GPS Module] -->|UART Serial RX/TX| ESP32
    ESP32 -->|I2C Data Bus Interface| LCD[LCD 1602 Character Display]
    ESP32 -->|GPIO Output Control| Buzzer[Piezo Buzzer Alert Module]
    ESP32 -->|PWM Speed Output Duty Cycles| Driver
    Driver -->|Regulated Motor Voltage| Motor[Geared DC Motors Vehicle Model]
    ESP32 -.->|Wi-Fi Telemetry Stream| Blynk[Blynk Cloud Infrastructure]
