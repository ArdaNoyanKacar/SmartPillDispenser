#include "configuration_mode.h"
#include "display.h"
#include "nrf_delay.h"
#include "button.h"
#include "dispension.h"
#include "rtc.h"
#include "dispenser_system.h"
#include <stdio.h>
#include <string.h>

// Constants for configuration (for adding a new dispension)
#define MAX_PILL_TYPES 2
#define MAX_AMOUNT 6

//#define TIME_INCREMENT 30  // minutes increment for time
#define TIME_INCREMENT 1

// Extended state machine states for configuration mode.
typedef enum {
    CONFIG_ADD_REMOVE,         // Top-level: choose between "Add" and "Remove"
    // Remove branch:
    CONFIG_REMOVE_PILL_TYPE,   // Select pill type to remove dispension
    CONFIG_REMOVE_DAY,         // Select day for removal
    CONFIG_REMOVE_LIST,        // List matching dispensions and select one for removal
    // Add branch:
    CONFIG_PILL_TYPE,          // Select pill type to add
    CONFIG_DAY,                // Select day for addition
    CONFIG_TIME,               // Select time for addition
    CONFIG_AMOUNT,             // Select amount for addition
    CONFIG_CONFIRM             // Confirmation screen for addition
} config_state_t;

// Strings for display.
static const char *day_names[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static const char *add_remove[2] = {"Add", "Remove"};

// Global variables for the removal branch.
static uint8_t rem_pill_type = 0;  // 0-indexed
static uint8_t rem_day = 0;        // 0-indexed: 0 = Sunday, etc.
static uint8_t matching_indices[MAX_DISPENSIONS]; // from dispension.h
static uint8_t match_count = 0;
static uint8_t rem_selection = 0;

// Global variables for the addition branch.
static uint8_t add_pill_type = 0;
static uint8_t add_day = 3;
static uint16_t add_time_minutes = 900; // default 8:00 AM
static uint8_t add_amount = 1;

#define STARTUP_TIME 900;
// Global option for top-level Add/Remove: 0 = Add, 1 = Remove.
static uint8_t option = 0;

//
// show_config_page()
//
// TDisplaysdisplays the current configuration screen based on the state.
static void show_config_page(config_state_t state)
{
    char buf[128];
    uint8_t hour = add_time_minutes / 60;
    uint8_t minute = add_time_minutes % 60;
    switch (state)
    {
        case CONFIG_ADD_REMOVE:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nSelect Option:\n< %s >", add_remove[option]);
            break;
        // --- Removal branch ---
        case CONFIG_REMOVE_PILL_TYPE:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nRemove: Select Pill:\n< %c >", 'A' + rem_pill_type);
            break;
        case CONFIG_REMOVE_DAY:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nRemove: Select Day:\n< %s >", day_names[rem_day % 7]);
            break;
        case CONFIG_REMOVE_LIST:
            if (match_count == 0) {
                snprintf(buf, sizeof(buf), "No dispensions\nfound for Pill %c on %s",
                         'A' + rem_pill_type, day_names[rem_day % 7]);
            } else {
                // Display the currently selected dispension details.
                dispension_t d = dispension_schedule[matching_indices[rem_selection]];
                snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nChoose:\nPill: %c\nTime: %02d:%02d\nAmt: %d",
                         'A' + d.pill_type, d.hour, d.minute, d.amount);
            }
            break;
        // --- Addition branch ---
        case CONFIG_PILL_TYPE:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nAdd: Select Pill:\n< %c >", 'A' + add_pill_type);
            break;
        case CONFIG_DAY:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nAdd: Select Day:\n< %s >", day_names[add_day % 7]);
            break;
        case CONFIG_TIME:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nAdd: Select Time:\n< %02d:%02d >", hour, minute);
            break;
        case CONFIG_AMOUNT:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nAdd: Select Amount:\n< %d >", add_amount);
            break;
        case CONFIG_CONFIRM:
            snprintf(buf, sizeof(buf), "SCHEDULE MENU\n_____________\n\nConfirm Add:\nPill: %c\nDay: %s\nTime: %02d:%02d\nAmt: %d",
                     'A' + add_pill_type, day_names[add_day % 7], hour, minute, add_amount);
            break;
        default:
            strcpy(buf, "Invalid State");
            break;
    }
    display_clear();
    display_text(buf);
}


// configuration_mode
//
// Implements the configuration state machine for both adding and removing dispensions.
void configuration_mode(void)
{
    config_state_t state = CONFIG_ADD_REMOVE;
    bool config_done = false;
    button_event_t button_event;
    rtc_time_t temp_current_time;

    // Initialize branch variables.
    option = 0;  // Default to Add.
    rem_pill_type = 0;
    rem_day = 0;
    match_count = 0;
    rem_selection = 0;
    add_pill_type = 0;
    add_day = 3;
    add_time_minutes = 900;
    add_amount = 1;

    while (!config_done)
    {
        // Run background tasks.
        if (rtc_get(&temp_current_time))
        {
            check_pill_dispense_schedule(&temp_current_time);
            update_day_of_week(&temp_current_time);
            update_dispense_and_pickup_timers();
        }

        // Update the display.
        show_config_page(state);

        
        button_event = button_get();

        if (button_event != BUTTON_NONE)
        {
            switch (state)
            {
                case CONFIG_ADD_REMOVE:
                    if (button_event == BUTTON_NEXT)
                    {
                        option = (option + 1) % 2;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        printf("Configuration cancelled!\n");
                        return;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        if (option == 0)
                            state = CONFIG_PILL_TYPE;  // Enter Add branch.
                        else
                            state = CONFIG_REMOVE_PILL_TYPE; // Enter Remove branch.
                    }
                    break;

                // --- Remove branch ---
                case CONFIG_REMOVE_PILL_TYPE:
                    if (button_event == BUTTON_NEXT)
                    {
                        rem_pill_type = (rem_pill_type + 1) % MAX_PILL_TYPES;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = CONFIG_ADD_REMOVE;
                        rem_pill_type = 0;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        state = CONFIG_REMOVE_DAY;
                    }
                    break;

                case CONFIG_REMOVE_DAY:
                    if (button_event == BUTTON_NEXT)
                    {
                        rem_day = (rem_day + 1) % 7;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = CONFIG_REMOVE_PILL_TYPE;
                        rem_day = 0;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        // Build list of matching dispensions.
                        match_count = 0;
                        for (uint8_t i = 0; i < num_dispensions; i++)
                        {
                            if (dispension_schedule[i].pill_type == rem_pill_type &&
                                dispension_schedule[i].day_of_week == rem_day)
                            {
                                matching_indices[match_count++] = i;
                            }
                        }
                        if (match_count == 0)
                        {
                            display_clear();
                            display_text("NO\nDISPENSIONS\nFound");
                            nrf_delay_ms(2000);
                            // Stay in this state to let the user choose again.
                        }
                        else
                        {
                            rem_selection = 0;
                            state = CONFIG_REMOVE_LIST;
                        }
                    }
                    break;

                case CONFIG_REMOVE_LIST:
                    if (button_event == BUTTON_NEXT)
                    {
                        rem_selection = (rem_selection + 1) % match_count;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = CONFIG_REMOVE_DAY;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        // Call the removal function (assumed to be dispension_remove(index)).
                        if (dispension_remove(matching_indices[rem_selection]))
                        {
                            printf("SUCCESS: Dispension removed!\n");
                        }
                        else
                        {
                            printf("ERROR: Could not remove dispension!\n");
                        }
                        config_done = true;
                        return;
                    }
                    break;

                // --- Add branch ---
                case CONFIG_PILL_TYPE:
                    if (button_event == BUTTON_NEXT)
                    {
                        add_pill_type = (add_pill_type + 1) % MAX_PILL_TYPES;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = CONFIG_ADD_REMOVE;
                        add_pill_type = 0;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        state = CONFIG_DAY;
                    }
                    break;

                case CONFIG_DAY:
                    if (button_event == BUTTON_NEXT)
                    {
                        add_day = (add_day + 1) % 7;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = CONFIG_PILL_TYPE;
                        add_day = 3;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        state = CONFIG_TIME;
                    }
                    break;

                case CONFIG_TIME:
                    if (button_event == BUTTON_NEXT)
                    {
                        add_time_minutes = (add_time_minutes + TIME_INCREMENT) % 1440;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        add_time_minutes = STARTUP_TIME;
                        state = CONFIG_DAY;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        state = CONFIG_AMOUNT;
                    }
                    break;

                case CONFIG_AMOUNT:
                    if (button_event == BUTTON_NEXT)
                    {
                        add_amount = (add_amount + 1) % (MAX_AMOUNT + 1);
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        add_amount = 0;
                        state = CONFIG_TIME;
                    }
                    else if (button_event == BUTTON_CONFIRM)
                    {
                        state = CONFIG_CONFIRM;
                    }
                    break;

                case CONFIG_CONFIRM:
                    if (button_event == BUTTON_CONFIRM)
                    {
                        dispension_t new_disp;
                        new_disp.pill_type = add_pill_type;
                        new_disp.day_of_week = add_day;
                        new_disp.hour = add_time_minutes / 60;
                        new_disp.minute = add_time_minutes % 60;
                        new_disp.amount = add_amount;
                        new_disp.dispensed = false;
                        if (dispension_add(new_disp))
                        {
                            printf("SUCCESS: New dispension added!\n");
                        }
                        else {
                            printf("WARNING: Failed to add dispension!\n");
                            display_clear();
                            display_text("Dispension ADD\n FAILED!\n\nRemove Old\nDispension"); 
                            nrf_delay_ms(2000);      
                        }
                        config_done = true;
                    }
                    else if (button_event == BUTTON_BACK)
                    {
                        state = CONFIG_AMOUNT;
                    }
                    break;

                default:
                    break;
            }
        }
        nrf_delay_ms(50);
    }
    printf("Exiting Configuration Mode.\n");
}