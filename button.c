#include "button.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include <microbit_v2.h>
#include "nrfx_gpiote.h"
#include "app_error.h"

// Define GPIO pins for the buttons (adjust these pin numbers as per your board)
#define LOAD_BUTTON_PIN      EDGE_P2   // Example pin for Load button
#define CONFIG_BUTTON_PIN    EDGE_P3   // Example pin for Configure button
#define NEXT_BUTTON_PIN      EDGE_P4   // Example pin for Next button
#define BACK_BUTTON_PIN      EDGE_P5   // Example pin for Back button
#define CONFIRM_BUTTON_PIN   EDGE_P6   // Example pin for Confirm button

volatile button_event_t latest_button = BUTTON_NONE;
/// OLD
/// OLD
/// OLD
// NO LONGER USED
// Initialize all button pins as inputs with pull-up resistors.
void button_init(void)
{
    nrf_gpio_cfg_input(LOAD_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(CONFIG_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(NEXT_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BACK_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(CONFIRM_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
}

// Interrupt handler for button actions
static void button_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{


    for (volatile uint32_t i = 0; i < 20000; i++)
    {

    }

    if (nrf_gpio_pin_read(pin) != 0)
    {
        return;
    }
    // Trigger on HIGH to LOW transition
    if (pin == LOAD_BUTTON_PIN)
        latest_button = BUTTON_LOAD;
    else if (pin == CONFIG_BUTTON_PIN)
        latest_button = BUTTON_CONFIG;
    else if (pin == NEXT_BUTTON_PIN)
        latest_button = BUTTON_NEXT;
    else if (pin == BACK_BUTTON_PIN)
        latest_button = BUTTON_BACK;
    else if (pin == CONFIRM_BUTTON_PIN)
        latest_button = BUTTON_CONFIRM;
}


// Initialize buttons with interrupts
// NEW
// NEW
// NEW
void button_interrupt_init(void)
{
    ret_code_t err;
    if (!nrfx_gpiote_is_init())
    {
        err = nrfx_gpiote_init();
        APP_ERROR_CHECK(err);
    }

    if (err != NRF_SUCCESS)
    {
        printf("Button initialization failed!\n");
    }

    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    config.pull = NRF_GPIO_PIN_PULLUP;

    err = nrfx_gpiote_in_init(LOAD_BUTTON_PIN, &config, button_interrupt_handler);
    if (err != NRF_SUCCESS)
    {
        printf("Button initialization failed!\n");
    }
    nrfx_gpiote_in_event_enable(LOAD_BUTTON_PIN, true);

    err = nrfx_gpiote_in_init(CONFIG_BUTTON_PIN, &config, button_interrupt_handler);
    if (err != NRF_SUCCESS)
    {
        printf("Button initialization failed!\n");
    }
    nrfx_gpiote_in_event_enable(CONFIG_BUTTON_PIN, true);

    err = nrfx_gpiote_in_init(NEXT_BUTTON_PIN, &config, button_interrupt_handler);
    if (err != NRF_SUCCESS)
    {
        printf("Button initialization failed!\n");
    }
    nrfx_gpiote_in_event_enable(NEXT_BUTTON_PIN, true);

    err = nrfx_gpiote_in_init(BACK_BUTTON_PIN, &config, button_interrupt_handler);
    if (err != NRF_SUCCESS)
    {
        printf("Button initialization failed!\n");
    }
    nrfx_gpiote_in_event_enable(BACK_BUTTON_PIN, true);

    err = nrfx_gpiote_in_init(CONFIRM_BUTTON_PIN, &config, button_interrupt_handler);
    if (err != NRF_SUCCESS)
    {
        printf("Button initialization failed!\n");
    }
    nrfx_gpiote_in_event_enable(CONFIRM_BUTTON_PIN, true);

    latest_button = BUTTON_NONE;
    printf("Button interrupts (simple) initialized.\n");
}










// Retrieve the current event and then clear it.
button_event_t button_get(void)
{
button_event_t event = latest_button;
latest_button = BUTTON_NONE;
return event;
}



// Poll buttons and return the first button event found.
// Buttons are assumed active low (pressed = 0).
button_event_t button_poll(void)
{
    // A simple polling routine with a debounce delay.
    if (nrf_gpio_pin_read(LOAD_BUTTON_PIN) == 0)
    {
        nrf_delay_ms(50);
        if (nrf_gpio_pin_read(LOAD_BUTTON_PIN) == 0)
            return BUTTON_LOAD;
    }
    if (nrf_gpio_pin_read(CONFIG_BUTTON_PIN) == 0)
    {
        nrf_delay_ms(50);
        if (nrf_gpio_pin_read(CONFIG_BUTTON_PIN) == 0)
            return BUTTON_CONFIG;
    }
    if (nrf_gpio_pin_read(NEXT_BUTTON_PIN) == 0)
    {
        nrf_delay_ms(50);
        if (nrf_gpio_pin_read(NEXT_BUTTON_PIN) == 0)
            return BUTTON_NEXT;
    }
    if (nrf_gpio_pin_read(BACK_BUTTON_PIN) == 0)
    {
            nrf_delay_ms(50);
        if (nrf_gpio_pin_read(BACK_BUTTON_PIN) == 0)
            return BUTTON_BACK;
    }
    if (nrf_gpio_pin_read(CONFIRM_BUTTON_PIN) == 0)
    {
        nrf_delay_ms(50);
        if (nrf_gpio_pin_read(CONFIRM_BUTTON_PIN) == 0)
            {
            return BUTTON_CONFIRM;
            printf("CONFIRM PRESSED!\n");
            }
    }
    return BUTTON_NONE;
}


