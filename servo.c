#include "servo.h"
#include "app_error.h"
#include <stdio.h>

// Use a single PWM instance for all three servos.
static nrfx_pwm_t pwm_instance = NRFX_PWM_INSTANCE(0);

// We'll use 4 channels; channel 3 remains unused.
static nrf_pwm_values_individual_t servo_pwm_values = {0, 0, 0, 0};

// Create a PWM sequence in individual load mode.
static nrf_pwm_sequence_t pwm_seq = {
    .values.p_individual = &servo_pwm_values,
    .length         = NRF_PWM_VALUES_LENGTH(servo_pwm_values),
    .repeats        = 0,
    .end_delay      = 0
};

/**
 * @brief Initialize the multi-channel PWM for servo control.
 */
static bool multi_servo_init(void)
{
    nrfx_pwm_config_t config = {
        .output_pins = {SERVO1_PIN, SERVO2_PIN, SERVO3_PIN, NRFX_PWM_PIN_NOT_USED},
        .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
        .base_clock   = NRF_PWM_CLK_1MHz,         // 1 µs resolution.
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 20000,                    // 20 ms period (50 Hz).
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,    // Individual duty cycle per channel.
        .step_mode    = NRF_PWM_STEP_AUTO
    };

    ret_code_t err = nrfx_pwm_init(&pwm_instance, &config, NULL);
    if (err != NRF_SUCCESS) {
        printf("ERROR: PWM initialization failed!\n");
        return false;
    }
    return true;
}

/**
 * @brief Calculate pulse width needed for motor and angle
 *
 * 
 */
static uint16_t calc_pulse(servo_channel_t motor_id, uint8_t angle)
{
    uint16_t pulse_width;
    if (motor_id == SERVO1)
    {
        pulse_width = 440 + ((angle * 2010) / 180);
    }
    else if (motor_id == SERVO2)
    {
        pulse_width = 340 + ((angle * 2010) / 180);
    }
    else
    {
        pulse_width = 440 + ((angle * 2010) / 180);
    }
    return pulse_width;
}

/**
 * @brief Initialize all servos.
 */
bool servo_init(void)
{
    bool success = multi_servo_init();
    // Reset all servos to 0° initially.
    set_servo_angle(SERVO1, 0);
    set_servo_angle(SERVO2, 0);
    return success;
}

/**
 * @brief Set the specified servo to the desired angle.
 *
 * Uses calc_pulse() for pulse width calculation.
 * The PWM register value is computed as (top_value - pulse_width).
 */
void set_servo_angle(servo_channel_t servo_id, uint8_t angle)
{
    uint16_t pulse_width = calc_pulse(servo_id, angle);
    // Top_value is 20000.
    uint16_t pwm_val = 20000 - pulse_width;

    switch (servo_id)
    {
        case SERVO1:
            servo_pwm_values.channel_0 = pwm_val;
            break;
        case SERVO2:
            servo_pwm_values.channel_1 = pwm_val;
            break;
        case SERVO3:
            servo_pwm_values.channel_2 = pwm_val;
            break;
        default:
            break;
    }

    // Restart PWM playback in loop mode so the signal is continuously output.
    nrfx_pwm_simple_playback(&pwm_instance, &pwm_seq, 1, NRFX_PWM_FLAG_LOOP);
    printf("Servo %d set to %d° (pulse: %dus, pwm value: %dus)\n", servo_id, angle, pulse_width, pwm_val);
}

/**
 * @brief Reset the specified servo to the centerposition.
 */
void servo_reset(servo_channel_t servo_id)
{
    set_servo_angle(servo_id, 90);
}

