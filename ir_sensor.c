#include "ir_sensor.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "app_error.h"
#include <stdio.h>

#define IR1_PIN EDGE_P0
#define IR2_PIN EDGE_P1

// Flags to store IR Sensor States
static volatile bool ir1_triggered = false;
static volatile bool ir2_triggered = false;

// Global callback pointer
static ir_sensor_callback_t sensor_callback = NULL;


// Register a callback function
void ir_sensor_register_callback(ir_sensor_callback_t callback)
{
    sensor_callback = callback;
}


/**
 * @brief Helper function that alerts the registered callback of an event
 * 
 * @param sensor_id ID of the sensor for the event
 * @param event Type of IR Sensor event
 */
static void alert_callback(uint8_t sensor_id, ir_event_t event)
{
    if (sensor_callback)
    {
        sensor_callback(sensor_id, event);
    }
}




/**
 * @brief Event handler for the IR Sensor pin
 * 
 * The handler is called on every edge. Reads the current pin state and
 * updates the ir_triggered flag
 */
static void ir_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pin == IR1_PIN){
        if (nrf_gpio_pin_read(pin) == 0)
        {
            // Pin is low: Beam is broken
            if (!ir1_triggered)  
            {
                ir1_triggered = true;
                printf("IR1 Sensor: Object Present!\n");
                alert_callback(0, IR_EVENT_DISPENSED);

                // TO DO:
                // Call a call-back function here to indicate the detection of a pill to the rest of the system
            }
        }
        // Pin is high: Beam is restored
        else{
            if (ir1_triggered)
            {
                ir1_triggered = false;
                printf("IR1 Sensor: Object is Removed!\n");
                alert_callback(0, IR_EVENT_TAKEN);

                // TO DO:
                // Call a call-back function here to indicate the removal of a pill to the rest of the system
            }
        }
    }
    // PIN 2
    else{
        if (nrf_gpio_pin_read(pin) == 0)
        {
            // Pin is low: Beam is broken
            if (!ir2_triggered)  
            {
                ir2_triggered = true;
                printf("IR2 Sensor: Object Present!\n");
                alert_callback(1, IR_EVENT_DISPENSED);

                // TO DO:
                // Call a call-back function here to indicate the detection of a pill to the rest of the system
            }
        }
        // Pin is high: Beam is restored
        else{
            if (ir2_triggered)
            {
                ir2_triggered = false;
                printf("IR2 Sensor: Object is Removed!\n");
                alert_callback(1, IR_EVENT_TAKEN);

                // TO DO:
                // Call a call-back function here to indicate the removal of a pill to the rest of the system
            }
        }
    }

}


bool ir_sensor_init(void)
{
    ret_code_t err;

    if (!nrf_drv_gpiote_is_init())
    {
        err = nrf_drv_gpiote_init();
        if (err != NRF_SUCCESS)
        {
            printf("ERROR: GPIOTE Module Could Not Be Initialized!\n");
            return false;
        }
    }

    // Create the config to be used by both pins
    nrf_drv_gpiote_in_config_t config;
    config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    config.pull = NRF_GPIO_PIN_PULLUP;
    config.is_watcher = false;
    config.hi_accuracy = true;
    config.skip_gpio_setup = false;

    
    // Initialize IR Sensor 1
    err = nrf_drv_gpiote_in_init(IR1_PIN, &config, ir_handler);

    if (err != NRF_SUCCESS)
    {
        printf("ERROR: Could nto initialize the IR1 Sensor pin!\n");
        return false;
    }

    nrf_drv_gpiote_in_event_enable(IR1_PIN,true);


    // Initialize IR Sensor 2
    err = nrf_drv_gpiote_in_init(IR2_PIN, &config, ir_handler);

    if (err != NRF_SUCCESS)
    {
        printf("ERROR: Could nto initialize the IR2 Sensor pin!\n");
        return false;
    }

    nrf_drv_gpiote_in_event_enable(IR2_PIN,true);

    return true;
}


bool ir1_sensor_read(void)
{
    // Return the state from the interrupt flag
    return ir1_triggered;
}


bool ir2_sensor_read(void)
{
    // Return the state from the interrupt flag
    return ir2_triggered;
}

bool ir_sensor_read(uint8_t sensor_id)
{
    if (sensor_id == 0)
    {
        return ir1_sensor_read();
    }
    else{
        return ir2_sensor_read();
    }
}