#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Button event types.
 */
typedef enum {
    BUTTON_NONE = 0,
    BUTTON_LOAD,
    BUTTON_CONFIG,
    BUTTON_NEXT,
    BUTTON_BACK,
    BUTTON_CONFIRM
} button_event_t;



extern volatile button_event_t latest_button;


/**
 * @brief Initialize all buttons.
 *
 * Configures the GPIO pins as inputs with pull-up resistors.
 */
void button_init(void);


/**
 * @brief Initialize all buttons.
 *
 * Configures the GPIO pins as inputs with pull-up resistors.
 */
void button_interrupt_init(void);


/**
 * @brief Poll all buttons and return the first detected button event.
 *
 * This function checks each button in order and returns the event corresponding to the first pressed button.
 *
 * @return button_event_t The button event, or BUTTON_NONE if no button is pressed.
 */
button_event_t button_poll(void);


/**
 * @brief Get latest button event that was triggered with an interrupt
 * 
 * @return button_event_t The button event, or BUTTON_NONE If no button is pressed
 */
button_event_t button_get(void);

#endif // BUTTON_H