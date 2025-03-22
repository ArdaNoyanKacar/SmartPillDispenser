#include <stdio.h>
#include "nrf_twi_mngr.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "rtc.h"
#include "i2c_driver.h"
#include "app_error.h"
#include "display.h"
#include "font8x8_basic.h"
#include "ir_sensor.h"
#include "servo.h"
#include "humidity.h"
#include "dispension.h"
#include "button.h"
#include "loading_mode.h"
#include "configuration_mode.h"
#include "dispenser_system.h"
#include "math.h"
#include "alarm_system.h"

// Number of containers
#define NUM_CONTAINERS 2
#define TEMP_HUMID_COUNTER_WAIT 300

// -------------------------------------
// System Modes
// -------------------------------------
typedef enum{
  MODE_NORMAL = 1,
  MODE_LOADING,
  MODE_CONFIGURATION
}system_mode_t;





static float old_temperature = 0;
static float old_humidity = 0;
static uint8_t old_minute = 0;





// Test functions
//
//-----------------------------------------------------------------------

// Test RTC functionality
void test_rtc(void)
{
  
  // Initialize the RTC driverr
  if (!rtc_init())
  {
    printf("RTC initialization failed\n");
    while(1);
  }
  else{
    printf("RTC initialization SUCCESS!\n");
  }


  // Set a known time on the DS3231
  rtc_time_t time_init = {2025, 3, 3, 4, 30, 0};
  if (!rtc_set(&time_init))
  {
    printf("Failed to set RTC time!\n");
    while (1);
  }
  else{
    printf("RTC time set SUCCESS!\n");
  }


  while (1)
  {
    rtc_time_t current_time;
    if (rtc_get(&current_time))
    {
      printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
          current_time.year,
          current_time.month,
          current_time.day,
          current_time.hour,
          current_time.minute,
          current_time.second);
    }
    else{
      printf("Failed to read RTC time!\n");
    }

    nrf_delay_ms(500);
  }
}

// Test Display functionality
void test_display(void)
{
  
  if (!display_init())
  {
    printf("Display couldn't be initialized!\n");
    while(1);
  }


  display_clear();

  if (!display_turn_on())
  {
    printf("Display turn on failed!\n");
    while(1);
  }
  else{
    printf("Display turn on successfull!\n");
  }


  display_text("Time: 3:55 \n Test 1 2 3\n     daada");

  //test_rtc(); // Test RTC functionality
}

// Test IR Sensor functionality
void test_ir_sensor(void)
{

  // Initialize the IR Sensors
  if (!ir_sensor_init())
  {
    printf("IR Sensor iniitalization failed!\n");
    while(1);
  }
  else
  {
    printf("IR Sensor initialization SUCCESS!\n");
  }

  while (1)
  {
    bool sensor_1 = ir1_sensor_read();
    //bool sensor_2 = ir2_sensor_read();

    if (sensor_1)
    {
      printf("SENSOR 1: Object Detected!\n");
    }
    else
    {
      printf("SENSOR1: Object not detected!\n");
    }

    /*
    if (sensor_2)
    {
      printf("SENSOR2: Object Detected!\n");
    }
    else
    {
      printf("SENSOR2: Object not detected!\n");
    }*/

    nrf_delay_ms(500);
  }
}

// Test Motor functionality
void test_motor(void)
{

  printf("Starting servo test...\n");

  if (!servo_init())
  {
      printf("Servo initialization failed!\n");
      while (1);
  }
  printf("Servo initialized.\n");

  
  while (1){

  set_servo_angle(SERVO1, 0);
  set_servo_angle(SERVO2, 0);
  
  nrf_delay_ms(2000);

  set_servo_angle(SERVO1, 45);
  set_servo_angle(SERVO2, 45);
  
  nrf_delay_ms(2000);

  set_servo_angle(SERVO1, 135);
  set_servo_angle(SERVO2, 135);
  
  nrf_delay_ms(2000);

  set_servo_angle(SERVO1, 180);
  set_servo_angle(SERVO2, 180);
  
  nrf_delay_ms(2000);
  }
}

// Test Humidity sensor functionality
void test_humidity()
{

    // Initialize the SHTC3 sensor.
    if (!humidity_init())
    {
        printf("SHTC3 initialization failed!\n");
        while(1);
    }
    else
    {
        printf("SHTC3 initialization SUCCESS!\n");
    }
   
    // Variables to hold measured values.
    float temperature = 0.0f, humidity = 0.0f;
   
    while (1)
    {
        if (humidity_measure(&temperature, &humidity))
        {
            printf("Temperature: %.2f °C, Humidity: %.2f %%RH\n", temperature, humidity);
        }
        else
        {
            printf("Failed to read SHTC3 sensor!\n");
        }
        nrf_delay_ms(2000);
    }
  }


//////////////////////////////////////////////////////////////////////////
// Main 
int main(void)
{

  // Initialize the dispenser, drivers and peripherals
  // I2C
  // RTC
  // Humidity
  // Servos
  // DIsplay
  // IR Sensors
  // Buttons
  if(!dispenser_init())
  {
    printf("ERROR: System could not be initialized!\n");
    while(1);
  }


  
  // Variables for measurements and time keeping
  rtc_time_t current_time;
  float temperature = 0.0;
  float humidity = 0.0;
  uint8_t measure_counter = 0; // To space out temp and humidity measurements
  bool recent_temp_humid_problem = false;
  uint32_t temp_humid_problem_counter = 0;

  

  // Main loop
  while (1)
  {
    
    button_event_t button_event = button_get();

    

    if (button_event == BUTTON_LOAD)
    {
      printf("Entering loading mode...\n");
      loading_mode();
    }
    else if (button_event == BUTTON_CONFIG)
    {
      printf("Entering schedule configuration...\n");
      configuration_mode();
    }

    // Read current rtc time
    if (rtc_get(&current_time))
    {
      printf("Time: %02d:%02d:%02d\n", current_time.hour, current_time.minute,
                                    current_time.second);
    }
    else{
      printf("ERROR: Failed to read RTC time!\n");
    }

    // Update the day of week if the rtc day changed
    


    // Background tasks for dispensing and checking if a pill has been taken
    check_pill_dispense_schedule(&current_time);
    update_day_of_week(&current_time);
    update_dispense_and_pickup_timers();


    if (button_event == BUTTON_NONE)
    {
      if (measure_counter % 10 == 0)
      {
        if (humidity_measure(&temperature, &humidity))
        {
          //printf("Temp: %.2fC  Humidity: %.2f%%RH\n", temperature, humidity);
        }
        else{
          printf("ERROR: Failed to measure temp and humidity!\n");
        }
      }
      measure_counter++;

      if(current_time.minute != old_minute || (fabsf(temperature - old_temperature ) > 0.01)|| (fabsf(temperature - old_temperature ) > 0.01))
      {
      update_display_info(&current_time, temperature, humidity);
      }

      if (recent_temp_humid_problem)
      {
        temp_humid_problem_counter++;
        printf("Counter: %ld\n", temp_humid_problem_counter);
        if (temp_humid_problem_counter == TEMP_HUMID_COUNTER_WAIT)
        {
          recent_temp_humid_problem = false;
          temp_humid_problem_counter = 0;
        }
      }


      if (recent_temp_humid_problem)
      {
        printf("TEMP HUMID PROBLEM!\n");
      }
      if ((temperature > 30 || humidity > 60) && !recent_temp_humid_problem)
      {
        alarm_bad_environment(temperature, humidity);
        recent_temp_humid_problem = true;
        temp_humid_problem_counter = 0;
      }

      // Update display tracking info
      old_humidity = humidity;
      old_minute = current_time.minute;
      old_temperature = temperature;
    }

    nrf_delay_ms(50);
  }


  return 0;

};