#pragma once

#include "nrf_delay.h"


// Get temperature value
// Non-blocking function. Calls callback with context when temperature is ready
void get_temperature_nonblocking(void (*callback)(float, void*), void* context);

// Get temperature value
// Blocking function. Returns temperature value when ready

void temp_callback(float temp, void* _unused);

void print_temp(void);
