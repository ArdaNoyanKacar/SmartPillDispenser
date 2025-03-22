// Temperature driver
//
// Accesses the internal temperature sensor peripheral

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"

#include "temperature.h"

volatile bool too_hot = false; 

static void (*callback_fn)(float, void*) = NULL;
static void* callback_context = NULL;

// Interrupt handler for Temperature sensor
void TEMP_IRQHandler(void) {
  NRF_TEMP->EVENTS_DATARDY = 0;

  // Be sure to only call function if non-null
  if (callback_fn) {
    float temperature = ((float)NRF_TEMP->TEMP)/4.0;
    callback_fn(temperature, callback_context);
  }
}

void get_temperature_nonblocking(void (*callback)(float, void*), void* context) {
  // Save callback
  callback_fn = callback;
  callback_context = context;

  // Enable lowest-priority interrupts
  NRF_TEMP->INTENSET = 1;
  NVIC_EnableIRQ(TEMP_IRQn);
  NVIC_SetPriority(TEMP_IRQn, 7);

  // Start temperature sensor
  NRF_TEMP->TASKS_START = 1;

  return;
}

typedef struct {
  float temp;
  bool flag;
} handler_info_t;



void temp_callback(float temp, void* _unused) {
  printf("non block Temperature: %f degrees C\n", temp);
  if (temp > 27.50) {

    too_hot = true;

  }
  else {
    too_hot = false;
  }
}


void print_temp(void) {


  // Get temperature without blocking
   get_temperature_nonblocking(temp_callback, NULL);
  nrf_delay_ms(1000); // should have printed before delay is complete
 
}
