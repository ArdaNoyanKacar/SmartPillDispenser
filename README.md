# Smart Pill Dispenser 
Arda Noyan Kacar - Carmine Bastone

An embedded system based on the Nordic nRF52 microcontroller for automated pill scheduling, dispensing, and consumption tracking. The device integrates multiple sensors, an RTC for scheduling, and a servo mechanism for dispensing, with an FSM-based control architecture and real-time user feedback.

---

## Features

- **RTC-based weekly scheduling** for timed dispensing events  
- **IR, temperature, and humidity sensors** for environment and dispense monitoring  
- **Servo-driven mechanism** capable of dispensing combinations of up to 4 pill types  
- **FSM-based control system** for reliable state management and error handling  
- **Pill consumption detection** integrated into the control logic  
- **OLED and buzzer alerts** providing immediate feedback after each dispense or failure  

---

## Hardware

- **MCU:** Nordic nRF52  
- **Sensors:** IR, temperature, and humidity modules  
- **RTC:** Real-time clock for weekly scheduling  
- **Actuator:** Servo motor for pill dispensing  
- **Display:** OLED module (I²C interface)  
- **Alert:** Buzzer for audible notifications  

---

## Firmware

- Written in **C**  
- Organized into modular components:  
  - `dispenser_system.c` – FSM-based control logic  
  - `alarm_system.c` – OLED and buzzer alert handling  
  - `ir_sensor.c`, `temperature.c`, `humidity.c` – sensor interfaces  
  - `rtc.c` – scheduling and timekeeping  
  - `servo.c` – mechanical dispensing control  

---

## Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/ArdaNoyanKacar/SmartPillDispenser.git
