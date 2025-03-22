#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <stdbool.h>
#include <stdint.h>
#include "microbit_v2.h"


#define IR1_PIN EDGE_P0
#define IR2_PIN EDGE_P1

/**
 * @brief IR Sensor event types
 */
typedef enum{
    IR_EVENT_NONE = 0,
    IR_EVENT_DISPENSED, // beam intact -> beam broken
    IR_EVENT_TAKEN      // beam broken -> beam intact
}ir_event_t;

/**
 * @brief Callback function prototype for IR sensor events
 * 
 * @param sensor_id ID of the sensor
 * @param event The event that occured
 */
typedef void (*ir_sensor_callback_t)(uint8_t sensor_id, ir_event_t event);


/**
 * @brief Registers a callback function as the IR sensor callback
 * 
 * @param callback Pointer to a function that will be called when an IR Sensor event occurs
 */
void ir_sensor_register_callback(ir_sensor_callback_t callback);


/**
 * @brief Initialize the IR break beam sensor
 * 
 * COnfigures the sensor pin as input and sets up a GPIOTE channel to trigger on both edges
 *      
 * @return True on success
 */
bool ir_sensor_init(void);

/**
 * @brief Read the current state of the IR1 break beam sensor
 * 
 * @return True if beam is broken, false otherwise
 */
bool ir1_sensor_read(void);



/**
 * @brief Read the current state of the IR2 break beam sensor
 * 
 * @return True if beam is broken, false otherwise
 */
bool ir2_sensor_read(void);



/**
 * @brief Read the current state of the desired IR sensor
 * 
 * @param sensor_id 1 -> SENSOR1, 2-> SENSOR2
 * @return True if the beam is broken, false otherwise
*/
bool ir_sensor_read(uint8_t sensor_id);


#endif