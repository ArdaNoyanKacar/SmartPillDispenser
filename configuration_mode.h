#ifndef CONFIGURATION_MODE_H
#define CONFIGURATION_MODE_H

#include "dispenser_system.h"

/**
 * @brief Run the Configuration Mode UI
 * 
 * This function handles the configuration state machine. It allows the user
 * to select the pil type, time, and dispension amount for a new schedule entry
 * Once all selections are confirmed a new dispension is added to the schedule
 * 
 * When the user presses back at the first stage, the mode is cancelled and system
 * returns to normal mode
 */
void configuration_mode(void);

#endif