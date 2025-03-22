#include "humidity.h"
#include "i2c_driver.h"
#include "nrf_error.h"
#include "nrf_delay.h"
#include <stdio.h>

// Helper function to send a 16bit command to the humidity sensor
static ret_code_t humidity_send_command(uint16_t command)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(command >> 8);
    buf[1] = (uint8_t)(command & 0xFF);
    return i2c_write_block(SHCT3_ADDRESS, buf, 2);
}


bool humidity_init(void)
{
    ret_code_t err;

    err = humidity_send_command(WAKE_COMMAND);
    if (err != NRF_SUCCESS)
    {
        printf("ERROR: Wake-up command failed!\n");
        return false;
    }
    // Wait a short time for the sensor to wake-up
    nrf_delay_ms(1);
    return true;
}

// Measure Humidity
bool humidity_measure(float *temp, float *humidity)
{
    ret_code_t err = humidity_send_command(MEASURE_COMMAND);

    if (err != NRF_SUCCESS)
    {
        printf("ERROR: Humidity measurement command failed!\n");
        return false;
    }
    // Wait for the measurement to happen
    nrf_delay_ms(15);

    uint8_t data_buf[6];

    err = i2c_read_block(SHCT3_ADDRESS, data_buf, 6);

    if (err != NRF_SUCCESS)
    {
        printf("ERROR: Humidity read failed!\n");
    }

    uint16_t raw_temp = ((uint16_t)data_buf[0] << 8) | data_buf[1];
    uint16_t raw_humidity =((uint16_t)data_buf[3] << 8) | data_buf[4];

    // Convert raw values using the datasheet formulas
    // Temperature = 
    // Humidity = 
    *temp = -45.0f + 175.0f * ((float)raw_temp / 65535.0f); 
    *humidity = 100.0f * ((float)raw_humidity / 65535.0f); 

    return true;
}


bool humidity_sleep(void)
{
    ret_code_t err = humidity_send_command(SLEEP_COMMAND);
    if (err != NRF_SUCCESS)
    {
        printf("ERROR: Humidity sleep command failed!\n");
        return false;
    }
    return true;
}
