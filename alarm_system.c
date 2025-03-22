#include "alarm_system.h"
#include "microbit_v2.h"
#include "nrf_gpio.h"
#include "display.h"
#include "button.h"    
#include "nrf_delay.h"
#include <stdio.h>


// Pin definitions
#define CONTAINER_FAIL_PIN          EDGE_P7      
#define LOW_CAPACITY_CONT_1_PIN     EDGE_P8
#define LOW_CAPACITY_CONT_2_PIN     EDGE_P9
#define PILL_PICKUP_PIN             EDGE_P10
#define BUZZER_PIN                  EDGE_P11  // For audible alerts


volatile bool dispense_alarm_notifier = false;

const char *container_names[2] = {"A", "B"};


/**
 * @brief Initializes the alarm system
 * 
 * COnfigures the LED and Buzzer pins
 */
bool alarm_init()
{
    nrf_gpio_cfg_output(CONTAINER_FAIL_PIN);
    nrf_gpio_cfg_output(LOW_CAPACITY_CONT_1_PIN);
    nrf_gpio_cfg_output(LOW_CAPACITY_CONT_2_PIN);
    nrf_gpio_cfg_output(PILL_PICKUP_PIN);
    nrf_gpio_cfg_output(BUZZER_PIN);

    nrf_gpio_pin_clear(CONTAINER_FAIL_PIN);
    nrf_gpio_pin_clear(LOW_CAPACITY_CONT_1_PIN);
    nrf_gpio_pin_clear(LOW_CAPACITY_CONT_2_PIN);
    nrf_gpio_pin_clear(PILL_PICKUP_PIN);
    nrf_gpio_pin_clear(BUZZER_PIN);

    return true;
}

/**
 * @brief Beep the buzzer once.
 */
static void beep_buzzer(void)
{
    nrf_gpio_pin_set(BUZZER_PIN);
    nrf_delay_ms(500);
    nrf_gpio_pin_clear(BUZZER_PIN);
}

/**
 * @brief Flashes and buzzes until confirmation of the issue is received
 * 
 * Shows what error occured that caused buzzing and flashing
 */
static void flash_and_buzz_until_ack(uint8_t led_pin, char buf[128])
{
    display_clear();
    display_text(buf);
    while (button_get() != BUTTON_CONFIRM) {
        nrf_gpio_pin_set(led_pin);
        nrf_gpio_pin_set(BUZZER_PIN);
        nrf_delay_ms(200);
        nrf_gpio_pin_clear(led_pin);
        nrf_gpio_pin_clear(BUZZER_PIN);
        nrf_delay_ms(200);
    }
}

/**
 * @brief Notifies the user that a pill has been dispensed.
 *
 * Pill pick up pill lights up solid and the buzzer is buzzed
 */
 void alarm_notify_dispense(uint8_t container_id)
 {  
    char msg[128];
    snprintf(msg, sizeof(msg),"NEW DISPENSION!\n\nCONTAINER:%s", container_names[container_id]);
    display_clear();
    display_text(msg);
    nrf_gpio_pin_set(PILL_PICKUP_PIN);
    beep_buzzer();
 }


 // Disables the dispension notification after the pill has been picked up
 void alarm_disable_notify_dispense()
{
    nrf_gpio_pin_clear(PILL_PICKUP_PIN);
}

// Intensifies buzzing and led flashing if a pill has not been picked up in enough time
void alarm_intensify_pickup(uint8_t container_id)
{
    char msg[128];
    sprintf(msg, "PILL NOT TAKEN\n\nCONTAINER: %s", container_names[container_id]);
    flash_and_buzz_until_ack(PILL_PICKUP_PIN, msg); 
}

/**
 * @brief Intensifies the alert when a dispensed pill is not picked up.
 *
 * This function continuously buzzes and flashes an LED until the user
 * acknowledges the alert by pressing the Confirm button.
 */
void alarm_container_low_capacity(dispension_t new_dispension)
{
    char msg[128];

    uint8_t hour = new_dispension.hour;
    uint8_t minute = new_dispension.minute;
    uint8_t container_id = new_dispension.pill_type;

    sprintf(msg, "LOW CAPACITY!\n\nCOULD NOT\nDISPENSE\n\nCONT: %s\nTIME: %02d:%02d\n", container_names[container_id], hour, minute);
    uint8_t led_pin = (container_id == 0) ? LOW_CAPACITY_CONT_1_PIN : LOW_CAPACITY_CONT_2_PIN;
    flash_and_buzz_until_ack(led_pin, msg);
}

/**
 * @brief Alerts that environmental conditions are out of range.
 */
void alarm_bad_environment(float temperature, float humidity)
{
    char msg[128];
    snprintf(msg,sizeof(msg), "BAD ENVIRONMENT\n\nTEMP:%.2fC\nHUM:%.2f", temperature, humidity);
    flash_and_buzz_until_ack(CONTAINER_FAIL_PIN, msg);
}

/**
 * @brief Alerts the user to a mechanical failure.
 */
void alarm_mechanical(uint8_t container_id)
{
    char msg[128];
    sprintf(msg, "MECH FAIL!\n\nCONT: %s", container_names[container_id]);
    flash_and_buzz_until_ack(CONTAINER_FAIL_PIN, msg);
}
