# Smart Pill Dispenser  

**Contributors:**  
- Arda Noyan Kacar  
- Carmine Bastone  

---

An embedded system based on the Nordic nRF52 microcontroller for automated pill scheduling, dispensing, and consumption tracking. The device integrates multiple sensors, an RTC for scheduling, and a servo mechanism for dispensing, with an FSM-based control architecture and real-time user feedback.

---

## Project Overview

![Smart Pill Dispenser](https://github.com/ArdaNoyanKacar/SmartPillDispenser/blob/d522cff688bb0918b30d228c45abe21a64b7c3f1/Pill%20Dispenser.jpg)

*(Prototype image – replace with the correct relative path to your picture file)*

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
