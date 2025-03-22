#ifndef DISPENSION_H
#define DISPENSION_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Structure that defines a dispensing event
 * 
 * @param pill_type // The type(container index) of the pill
 * @param day_of_week // Day to be dispensed
 * @param hour // Hour to be dispensed
 * @param minute // Minute to be dispensed
 * @param amount // Amount to be dispensed
 * @param dispensed // Flag to notify dispension occured recently, set back to false after some time
 */
typedef struct{
    uint8_t pill_type;
    uint8_t day_of_week;
    uint8_t hour;
    uint8_t minute;
    uint8_t amount;
    uint8_t dispensed;
}dispension_t;


#define MAX_DISPENSIONS 50


/**
 * @brief Global array holding dispensing scheduling events
 */
extern dispension_t dispension_schedule[MAX_DISPENSIONS];


/**
 * @brief Number of dispension events currently in schedule
 */
extern uint8_t num_dispensions;


/**
 * @brief Add a new dispension event to the schedule
 * 
 * @param new_dispension The new dispension event to add
 * @return True if added successfully, false otherwise
 */
bool dispension_add(dispension_t new_dispension);


/**
 * @brief Remove a dispension event from the schedule
 * 
 * @param index Index of the event to remove.
 * @return True if removed successfully, false otherwise
 */
bool dispension_remove(uint8_t index);

#endif 