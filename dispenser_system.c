#include "dispenser_system.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "i2c_driver.h"
#include "rtc.h"
#include "display.h"
#include "humidity.h"
#include "ir_sensor.h"
#include "servo.h"
#include "dispension.h"
#include "button.h"
#include <stdio.h>
#include "alarm_system.h"

uint16_t container_counts[NUM_CONTAINERS] = {0, 0};
volatile bool pill_waiting[NUM_CONTAINERS] = {false, false};
volatile uint32_t pill_wait_timer[NUM_CONTAINERS] = {0, 0};
volatile bool dispension_in_progress[NUM_CONTAINERS] = {false, false};
volatile uint32_t dispension_confirm_timer[NUM_CONTAINERS] = {0, 0};
uint16_t servo_angle[] = {0, 0};



// Also, assume you have defined the number of containers and timeouts:
#define NUM_CONTAINERS_LOCAL 2


// Global variables for system-level tracking:
static uint8_t current_day_of_week = 3;  // Assume RTC is set with day-of-week = 1 (e.g., Monday).
static uint8_t last_rtc_day = 0;



// Callback function to register to the IR sensor
void ir_sensor_callback(uint8_t sensor_id, ir_event_t event)
{
  if (event == IR_EVENT_TAKEN)
  {
    if (pill_waiting[sensor_id])
    {
      printf("IR Sensor detected pill pickup at container: %d", sensor_id );
      pill_waiting[sensor_id] = false;
      alarm_disable_notify_dispense();
      nrf_delay_ms(50);
      pill_wait_timer[sensor_id] = 0;
    }

  }
}



//----------------------------------------------------------------------
// dispenser_init()
// Initializes all peripherals and drivers.
bool dispenser_init(void)
{
    printf("Smart Pill Dispenser Starting...\n");

    ret_code_t err = i2c_init();
    if (err != NRF_SUCCESS)
    {
        printf("ERROR: I2C initialization failed!\n");
        return false;
    }
    printf("I2C initialized.\n");

    if (!rtc_init())
    {
        printf("ERROR: RTC initialization failed!\n");
        return false;
    }
    printf("RTC initialized.\n");

    nrf_delay_ms(100);

    rtc_time_t rtc_init_time;
    if (rtc_get(&rtc_init_time))
    {
        last_rtc_day = rtc_init_time.day;
    }

    if (!display_init())
    {
        printf("ERROR: Display initialization failed!\n");
        return false;
    }

    display_clear();
    printf("Display initialized.\n");

    
    if (!display_turn_on())
  {
    printf("Display turn on failed!\n");
    while(1);
  }
  else{
    printf("Display turn on successfull!\n");
    
  }
  display_text("TDisplay initialized\n");

    if (!humidity_init())
    {
        printf("ERROR: Humidity sensor initialization failed!\n");
        return false;
    }
    printf("Humidity sensor initialized.\n");

    if (!ir_sensor_init())
    {
        printf("ERROR: IR sensor initialization failed!\n");
        return false;
    }
    // Register your IR sensor callback (assumed implemented elsewhere)
    ir_sensor_register_callback(ir_sensor_callback);
    printf("IR sensor initialized.\n");

    if (!servo_init())
    {
        printf("ERROR: Servo initialization failed!\n");
        return false;
    }
    printf("Servo initialized.\n");

    // Initialize buttons
    button_init();
    printf("Buttons initialized.\n");

    button_interrupt_init();
    printf("Button interrupts initialized.\n");


    if (!alarm_init())
    {
        printf("ERROR: Alarm system initialization failed!\n");
        return false;
    }
    printf("Alarm system initialized!\n");

    

    return true;
}

//----------------------------------------------------------------------
// update_day_of_week()
// Called each loop to check if the RTC day has changed. If so, update
// the global current_day_of_week and reset the dispensed flags in the schedule.
void update_day_of_week(const rtc_time_t *current_time)
{
    if (current_time->day != last_rtc_day)
    {
        current_day_of_week = (current_day_of_week + 1) % 7;
        last_rtc_day = current_time->day;
        printf("Day rollover: new day-of-week = %d\n", current_day_of_week);
        for (uint8_t i = 0; i < num_dispensions; i++)
        {
            dispension_schedule[i].dispensed = false;
        }
    }
}

//----------------------------------------------------------------------
// dispense()
// A helper function that triggers the servo to dispense pills.
// Here we simply set the servo angle based on the pill type and amount.
void dispense(uint8_t pill_type, uint8_t amount)
{
    // Example: Set servo angle; adjust as needed.
    printf("Dispension called, Amount: %d!\n", amount);
    printf("DISPENSE\nDISPENSE\nDISPENSE\nDISPENSE\n");
    printf("Current servo angle: %d\n", servo_angle[pill_type]);
    for (uint8_t i = 0; i < amount; i++)
    {
    if (servo_angle[pill_type] >= 150)
    {
        servo_angle[pill_type] += 25;
    }
    else{
        servo_angle[pill_type] += 30;
    }
    
    printf("Servo Angle: %d\n", servo_angle[pill_type]);
    set_servo_angle(pill_type, servo_angle[pill_type]);
    nrf_delay_ms(500);
    }
    nrf_delay_ms(2000);
    if (container_counts[pill_type] == 0)
    {
        set_servo_angle(pill_type, 0);
        servo_angle[pill_type] = 0;
    }
}

//----------------------------------------------------------------------
// check_pill_dispense_schedule()
// Checks the dispensing schedule against the current time and triggers dispensing events.
void check_pill_dispense_schedule(const rtc_time_t *current_time)
{
    //printf("Checking, dispense schedule. Num dispensions: %d\n", num_dispensions);
    for (uint8_t i = 0; i < num_dispensions; i++)
    {
        //printf("DIspension|| Day: %d, Hour: %d, Minute: %d\n", dispension_schedule[i].day_of_week, dispension_schedule[i].hour, dispension_schedule[i].minute);
        //printf("Curret time: Day: %d,Hour: %d, Minute: %d\n", current_day_of_week, current_time->hour, current_time->minute);
        /*if (dispension_schedule[i].dispensed)
        {
            printf("Recently dispensed\n");
        }*/
        if ((dispension_schedule[i].day_of_week == current_day_of_week) &&
            (dispension_schedule[i].hour == current_time->hour) &&
            (dispension_schedule[i].minute == current_time->minute) &&
            (!dispension_schedule[i].dispensed))
        {
            printf("Schedule match: Dispensing pill type %d, amount %d\n",
                   dispension_schedule[i].pill_type, dispension_schedule[i].amount);
           

           
            
            uint8_t container_index = dispension_schedule[i].pill_type;
            if (container_index < NUM_CONTAINERS_LOCAL)
            {
                if (container_counts[container_index] >= dispension_schedule[i].amount)
                {
                    container_counts[container_index] -= dispension_schedule[i].amount;
                    dispense(dispension_schedule[i].pill_type, dispension_schedule[i].amount);
                    
                    
                    dispension_in_progress[container_index] = true;
                    dispension_confirm_timer[container_index] = DISPENSION_CONFIRM_TIMEOUT;
                    
                }
                else
                {
                    printf("ALERT: Container %d is empty!\n", container_index);
                    alarm_container_low_capacity(dispension_schedule[i]);
                }
                dispension_schedule[i].dispensed = true;
                
            }
        }
    }
}

//----------------------------------------------------------------------
// update_dispense_and_pickup_timers()
// Updates the timers for confirming a dispense and for waiting for a pill pickup.
void update_dispense_and_pickup_timers(void)
{
    for (uint8_t i = 0; i < NUM_CONTAINERS_LOCAL; i++)
    {
        // Dispense confirmation update.
        if (dispension_in_progress[i])
        {
            printf("\n\nDispension in progress for container: %d\n\n", i);
            if (ir_sensor_read(i))  // Assumes a function that reads sensor state by container index.
            {
                printf("IR confirms pill in container %d.\n", i);
                pill_waiting[i] = true;
                pill_wait_timer[i] = PILL_WAIT_TIMEOUT;
                dispension_in_progress[i] = false;

            }
            else
            {
                if (dispension_confirm_timer[i] > 0)
                {
                    dispension_confirm_timer[i]--;
                    if (dispension_confirm_timer[i] % 2 == 0)
                        printf("Waiting for confirmation in container %d: %lu sec remaining\n",
                               i, (unsigned long)dispension_confirm_timer[i]);
                }
                else
                {
                    printf("ALERT: No pill detected in container %d after dispensing!\n", i);
                    dispension_in_progress[i] = false;
                    alarm_mechanical(i);
                }
            }
        }
       
        // Pill pickup timer update.
        if (pill_waiting[i])
        {   
            
            if (pill_wait_timer[i] > 0)
            {   
                alarm_notify_dispense(i);
                pill_wait_timer[i]--;
                if (pill_wait_timer[i] % 5 == 0)
                    printf("Container %d pickup: %lu sec remaining\n", i + 1, (unsigned long)pill_wait_timer[i]);
            }
            else
            {
                printf("ALERT: Pill in container %d not picked up!\n", i + 1);
                pill_waiting[i] = false;
                alarm_intensify_pickup(i);
            }
        }
    }
}

//----------------------------------------------------------------------
// update_display_info()
// Updates the status display in Normal mode.
void update_display_info(const rtc_time_t *current_time, float temperature, float humidity)
{
    char buf[128];
    snprintf(buf, sizeof(buf),
             "SMART DISPENSER\n_______________\n\nTime: %02d:%02d\nTemp  %.1fC\nHumid: %.1f%%\nCounts: %d %d",
             current_time->hour, current_time->minute,
             temperature, humidity,
             container_counts[0], container_counts[1]);
    display_clear();
    display_text(buf);
}

