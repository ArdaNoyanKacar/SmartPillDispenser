#ifndef DISPENSER_SYSTEM_H
#define DISPENSER_SYSTEM_H

#include <stdbool.h>
#include <stdint.h>
#include "rtc.h"

#define NUM_CONTAINERS 2

// ---------------------------------------
// Container Pill Counts
// ---------------------------------------
// Assume each container holds one type of pill
extern uint16_t container_counts[NUM_CONTAINERS];


extern uint16_t servo_angle[2];
// -----------------------------------------
// Global variables for pill pickup tracking
// -----------------------------------------
#define DISPENSION_CONFIRM_TIMEOUT 20
#define PILL_WAIT_TIMEOUT 30


extern volatile bool pill_waiting[NUM_CONTAINERS];
extern volatile uint32_t pill_wait_timer[NUM_CONTAINERS];
extern volatile bool dispension_in_progress[NUM_CONTAINERS];
extern volatile uint32_t dispension_confirm_timer[NUM_CONTAINERS];




// Initializes all system peripherals and drivers.
// Returns true if initialization is successful.
bool dispenser_init(void);

// Background update functions that must be called regularly.
void update_day_of_week(const rtc_time_t *current_time);
void check_pill_dispense_schedule(const rtc_time_t *current_time);
void update_dispense_and_pickup_timers(void);
void update_display_info(const rtc_time_t *current_time, float temperature, float humidity);

// A helper function that triggers the servo for dispensing.
void dispense(uint8_t pill_type, uint8_t amount);

#endif // DISPENSER_SYSTEM_H