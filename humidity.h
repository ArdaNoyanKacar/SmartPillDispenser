#ifndef HUMIDITY_H
#define HUMIDITY_H


#include <stdint.h>
#include <stdbool.h>



#define SHCT3_ADDRESS 0X70
#define SLEEP_COMMAND 0xB098
#define WAKE_COMMAND 0x3517
#define MEASURE_COMMAND 0x7866 // Clock-stretching disabled
#define READ_RH_FIRST 0x58E0

/**
 * @brief Initialize the humidity sensor
 * 
 * @return True if intitialization successful, false otherwise
 */
bool humidity_init(void);

/**
 * @brief Perform a measurement
 * 
 * Reads temperature and relative humidity from the sensor
 * 
 * @param temp Pointer to a float to store the temperature in C
 * @param humidity Pointer to a gloat to store humidity (%RH)
 * @return True if measurement and data conversion succeeded, false otherwise
 */
bool humidity_measure(float *temp, float *humidity);

/**
 * @brief Put the sensor to sleep
 * 
 * @return True if successful, false otherwise
 */
bool humidity_sleep(void);

#endif