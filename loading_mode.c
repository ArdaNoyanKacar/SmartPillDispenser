#include "loading_mode.h"
#include "button.h"
#include "display.h"
#include "nrf_delay.h"
#include <stdio.h>
#include <stdint.h>
#include "dispenser_system.h"

extern uint16_t container_counts[];


// States for loading mode
typedef enum {
    LOAD_CONTAINER_SELECT,
    LOAD_ENTER_COUNT,
    LOAD_CONFIRM
}load_state_t;

// Constants
#define NUM_CONTAINERS 2
#define CONTAINER_CAPACITY 6

// Container Names
static const char *container_names = "AB";

// Display the container seleciton page
static void show_container_selection(uint8_t selected)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "LOADING MENU\n____________\n\nSelect Cont:\n< %c >", container_names[selected]);
    display_clear();
    display_text(buf);
}

// Display the amount selection page
static void show_amount_selection(uint8_t amount)
{
    char buf[128];
    snprintf(buf, sizeof(buf),"LOADING MENU\n____________\n\nSelect Amount:\n < %d >", amount);
    display_clear();
    display_text(buf);
}

// Display the confirmation page
static void show_confirmation(uint8_t container_index, uint8_t amount)
{
    printf("Confirmation screen shown!\n");
    char buf[128];
    snprintf(buf, sizeof(buf), "LOADING MENU\n____________\n\nConfirm Loading:\nCont: %c\nAmount: %d",
                                container_names[container_index], amount);
    display_clear();
    display_text(buf);
}


// Implements the loading mode state machine
void loading_mode(void)
{
    load_state_t state = LOAD_CONTAINER_SELECT;
    uint8_t selected_container = 0;
    uint8_t amount = 1;

    bool loading_complete = false;
    button_event_t button_event;
    rtc_time_t temp_current_time;

   

    while (!loading_complete)
    {
        // Run background tasks
        if (rtc_get(&temp_current_time))
        {
            check_pill_dispense_schedule(&temp_current_time);
            update_day_of_week(&temp_current_time);
            update_dispense_and_pickup_timers();
        }
        switch (state)
        {
            case LOAD_CONTAINER_SELECT:
                show_container_selection(selected_container);
                break;
            case LOAD_ENTER_COUNT:
                show_amount_selection(amount);
                break;
            case LOAD_CONFIRM:
                show_confirmation(selected_container, amount);
            
            default:
                break;
        }
        //button_event = button_poll();
        button_event = button_get();

        if (button_event != BUTTON_NONE)
        {
            switch (state)
            {
                case LOAD_CONTAINER_SELECT:
                    if (button_event == BUTTON_NEXT)
                    {
                        selected_container = (selected_container + 1) % NUM_CONTAINERS;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        printf("Loading cancelled!\n");
                        return;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        state = LOAD_ENTER_COUNT;
                        amount = 1;
                    }
                    break;

                case LOAD_ENTER_COUNT:

                    if (button_event == BUTTON_NEXT)
                    {
                        uint8_t missing;

                        if (container_counts[selected_container] < CONTAINER_CAPACITY)
                        {
                            missing = CONTAINER_CAPACITY - container_counts[selected_container];
                        }
                        else{
                            missing = 0;
                        }
                        if (missing != 0)
                        {
                            amount = (amount + 1) % (missing + 1);
                        }
                        else{
                            amount = 0;
                        }
                        break;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = LOAD_CONTAINER_SELECT;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        state = LOAD_CONFIRM;
                    }
                    break;
                case LOAD_CONFIRM:
                    if (button_event == BUTTON_CONFIRM)
                    {
                        
                        printf("Selected Container: %d", selected_container);
                        show_confirmation(selected_container, amount);
                        container_counts[selected_container] += amount;
                        printf("Pills Loaded!\nContainer %d Updated!\nNew amount: %d\n",selected_container, container_counts[selected_container]);
                        return;
                        
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = LOAD_ENTER_COUNT;
                    }
                    break;
                default:
                    break;
            }   
        }
            nrf_delay_ms(50);
    }

    printf("Exiting loading mode, returning to normal mode\n");
}
