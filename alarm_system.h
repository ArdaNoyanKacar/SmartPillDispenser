#ifndef ALARM_SYSTEM_H
#define ALARM_SYSTEM_H

#include <stdbool.h>
#include <stdint.h>
#include "dispension.h"

extern volatile bool dispense_alarm_notifier;

// Initialize the alarm system hardware.
bool alarm_init(void);

/**
 * @brief LIghts up a green LED to indicate the arrival of new dispensions
 * BUzzes infrequently 
 */
void alarm_notify_dispense(uint8_t container_id);


/**
 * @brief Disables the pill pickup LED 
 * 
 * Called when there is a successfull pickup
 */
void alarm_disable_notify_dispense();



// Intensify the alert for a dispensed pill that has not been picked up.
// This function continuously buzzes and flashes an LED until the user acknowledges.
void alarm_intensify_pickup(uint8_t container_id);

// Other alarm functions, e.g. for mechanical failure, container low capacity, or bad environment.
void alarm_mechanical(uint8_t container_id);
void alarm_container_low_capacity(dispension_t new_dispension);
void alarm_bad_environment(float temperature, float humidity);


#endif // ALARM_SYSTEM_H
