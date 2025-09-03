# Smart Pill Dispenser
## Authors

- **Arda Noyan Kacar**  
- **Carmine Bastone**
   
An embedded, on-device system for **weekly medication scheduling, dispensing, and verification**. Runs on a Nordic **nRF52** MCU with **IR pill detection**, **temperature/humidity monitoring**, **RTC-based scheduling**, an **OLED UI**, a **buzzer**, and a **servo** mechanism for dispensing.

![Device](./Pill%20Dispenser.jpg)

---

## What it does (at a glance)

- **Schedules** doses per a weekly plan with **multiple windows per day**.
- **Handles complex regimens**: same-time **combinations of pills** and pills on **different intervals** (e.g., daily + Mon/Wed/Fri + every 8 hours). When intervals align, the device dispenses a **combined set**; otherwise, pills are dispensed in **separate windows**.
- **Verifies delivery** with an IR sensor and **prompts pickup** via display and buzzer.
- **Guards safety** with environment checks, jam/stall detection, sensor-fault handling, and safe recovery on power loss.
- **Supports on-device setup** and a **safe refilling mode** so normal dispensing can’t occur while you service the device.

Core control: `dispenser_system.c` · Safe refilling: `loading_mode.c` · On-device setup: `configuration_mode.c`

---

## Scheduling: combinations and intervals

The dispenser is built for **real-world medication routines**, not a single pill at a fixed time.

- **Weekly plan** with configurable **dose windows** per day.
- **Same-time combinations**: a window can dispense up to **four pill types** together as a single pickup event.
- **Independent cadences**: each pill can follow its own cadence (daily, specific weekdays, every N hours). The controller merges or separates windows automatically.
- **Multiple doses per day** supported (morning / noon / evening / nightly or custom times).
- **Catch-up on boot**: if the device powers up inside a window, it resumes at the correct point **without double-dispensing**.
- **Snooze or skip** within an active window; both actions are recorded for adherence tracking.

---

## How a scheduled dose runs

1. **Window opens**  
   The RTC signals a due window. If boot occurs during a window, the controller resumes correctly.

2. **Pre-checks**  
   - Not in **Loading** or **Configuration** mode  
   - Optional **temperature/humidity** within limits  
   - Optional **IR baseline** (tray clear) before motion

3. **Actuation**  
   The servo executes a motion profile to dispense the scheduled item(s) (single pill or combination), then returns to a safe/home position. Timeouts capture stalls/jams.

4. **Verify and prompt**  
   The IR sensor must confirm pill(s) present in the pickup area within a short verification window. The OLED and buzzer prompt the user to collect.

5. **Pickup and closure**  
   If the pill(s) remain detected beyond a pickup timeout, the device re-alerts and logs the outcome.

---

## Scenarios the system handles

- **No pill detected after motion**  
  Performs a small corrective move and/or **retry** (configurable). If still absent, alerts and logs a failed attempt for that window.

- **Mechanical jam or servo stall**  
  If expected positions are not reached by timeout: stop motion, alert, and require manual recovery before resuming.

- **IR sensor fault (stuck high/low)**  
  Treat as a sensor issue; **inhibit dispensing**, prompt the user to check the chute/sensor.

- **Temperature/humidity out of range**  
  If enabled, either **warn** or **block dispensing** until conditions are acceptable.

- **RTC invalid or time not set**  
  Scheduling is paused; the UI prompts to set the clock in Configuration Mode.

- **Power loss mid-cycle**  
  On restart, return to a safe state; do **not** re-dispense blindly. Resume if the window is still active, or log an **interrupted** attempt.

- **User abort**  
  User can cancel; motion returns to a safe position and the event is recorded.

---

## Modes on the device

### Normal Operation
Runs the scheduled dose flow during weekly windows.  
(Logic: `dispenser_system.c`)

### Loading Mode (safe refilling)
- **Disables** scheduled dispensing so you can refill/service safely  
- Allows **manual servo jog** to test motion or clear obstructions  
- Must be explicitly exited to re-enable normal operation  
(Logic: `loading_mode.c`)

### Configuration Mode (on-device setup)
- Set **date/time** and define the **weekly schedule**, including **combinations** and **per-pill intervals**  
- Adjust behavior: verification and pickup timeouts, retry counts, jam timeout, and environment thresholds  
- While editing, scheduled dispensing is held to keep settings consistent  
(Logic: `configuration_mode.c`)

---

## Technical details

### Sensors

- **IR pill detection**  
  - Reflective IR emitter/receiver near the pickup area  
  - Firmware samples a baseline, applies a threshold with debounce to confirm presence  
  - **Verification window** and **debounce** durations are configurable  
  - **Fault detection** identifies “stuck high/low” conditions and blocks dispensing until cleared

- **Temperature and humidity**  
  - I²C-based sensor polled at boot, before dispense, and on demand  
  - Thresholds are configurable (warn or block behavior)  
  - Values displayed via the UI when relevant (e.g., warning banners)

### Real-Time Clock (RTC)

- Maintains **weekly schedules** with minute-level resolution  
- Must be **set once** after flashing or power loss; persists across resets  
- If time is invalid, dispensing is **inhibited** and the UI requests configuration  
- Device resumes correctly when powered up **inside** a valid window

### Actuation (Servo)

- Standard hobby-servo control via PWM (≈50 Hz)  
- Typical pulse widths around 1.0–2.0 ms (configurable endpoints)  
- Motion profile covers **index → drop → home** for each event  
- **Timeout-based jam detection** halts motion and raises an alert  
- Designed to return to a **safe/home** position on abort or error

### User interface

- **OLED display** (I²C) for prompts, warnings, environment status, and mode guidance  
- **Buzzer** with distinct patterns for reminders vs warnings (jam, sensor fault, no-pill)  
- **Buttons** with short-press acknowledgements and long-press mode entry/exit

### Power and reliability

- Brownout-safe behavior: cycles are not repeated automatically after a power dip  
- Practical note: servos can draw transient current; provide adequate decoupling and follow your board’s power guidance to avoid resets

---

## Quick start

1. **Set time and plan**  
   Enter **Configuration** to set the RTC, define weekly windows, and assign pill **combinations** or **intervals** per pill type.

2. **Refill safely**  
   Enter **Loading** to refill; jog the servo if needed; **exit** Loading to re-enable normal operation.

3. **Run**  
   The device dispenses per window, verifies via IR, and prompts pickup. Follow on-screen and buzzer prompts.

4. **Respond to alerts**  
   Address no-pill, jam, sensor fault, environment warnings, or missed pickup as indicated by the UI.

---


