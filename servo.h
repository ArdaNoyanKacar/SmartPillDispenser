#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <stdbool.h>
#include "microbit_v2.h"
#include <nrfx_pwm.h>

// Define servo output pins.
#define SERVO1_PIN   EDGE_P13
#define SERVO2_PIN   EDGE_P15
#define SERVO3_PIN   EDGE_P16

// Use an enumeration for servo channels.
typedef enum {
    SERVO1 = 0,
    SERVO2,
    SERVO3
} servo_channel_t;

/**
 * @brief Initializes the multi-channel PWM for servo control.
 *
 * @return true if initialization is successful, false otherwise.
 */
bool servo_init(void);

/**
 * @brief Sets the specified servo to the desired angle.
 *
 * Uses your original pulse width calculation.
 *
 * @param servo_id The servo to adjust (SERVO1, SERVO2, or SERVO3).
 * @param angle    The desired angle (0-180).
 */
void set_servo_angle(servo_channel_t servo_id, uint8_t angle);

/**
 * @brief Resets the specified servo to the center (90°) position.
 *
 * @param servo_id The servo to reset.
 */
void servo_reset(servo_channel_t servo_id);

#endif // SERVO_H

