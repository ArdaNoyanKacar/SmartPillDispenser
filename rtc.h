#ifndef RTC_H
#define RTC_H
#include <stdint.h>
#include <stdbool.h>
#include "nrf_error.h"
#include "i2c_driver.h"

// Macros
#define DS3132_ADDRESS 0x68
#define DS3231_TIME 0X00
#define DS3231_ALARM1 0X07 
#define DS3231_ALARM2 0X0B
#define DS3231_CONTROL 0x0E
#define DS323_STATUS 0X0F

// Data structure to store time of day information
typedef struct{
    uint16_t year; // Full year
    uint8_t month; //1-12
    uint8_t day; //1-31
    uint8_t hour; //0-23
    uint8_t minute; //0-59
    uint8_t second; // 0-59
} rtc_time_t;


// Alarm mode enums for ALARM1 (includes seconds)
typedef enum{
    RTC_ALARM1_PER_SECOND,                      // Alarm every second
    RTC_ALARM1_MATCH_SECONDS,                   // Match seconds only
    RTC_ALARM1_MATCH_MINUTES_SECONDS,           // Match minutes and seconds
    RTC_ALARM1_MATCH_HOURS_MINUTES_SECONDS,     // Match hours, minutes and seconds
    RTC_ALARM1_MATCH_DATE_HOURS_MINUTES_SECONDS // Match date, hours, minutes and seconds
} rtc_alarm1_mode_t;


// Alarm mode enums for ALARM2 (does not inlcude seconds)
typedef enum{
    RTC_ALARM2_PER_MINUTE,              // Alarm every minute
    RTC_ALARM2_MATCH_MINUTES,           // Match minutes 
    RTC_ALARM2_MATCH_HOURS_MINUTES,     // Match hours and minutes
    RTC_ALARM2_MATCH_DATE_HOURS_MINUTES // Match date, hours and minutes
} rtc_alarm2_mode_t;








// Initialize the DS3231 RTC Module
// It will return a false if the I2C manager has not been set
bool rtc_init(void);

// Set the RTC with the provided time of the day
bool rtc_set(const rtc_time_t *time);

// Retrieve the current time from the RTC
bool rtc_get(rtc_time_t *time);

// Set ALARM1 using a given time and mode
bool rtc_set_alarm1(rtc_time_t *time, rtc_alarm1_mode_t mode);

// Set ALARM2 using a given time and mode
bool rtc_set_alarm2(rtc_time_t *time, rtc_alarm2_mode_t mode);

// Clear the RTC alarm flags
bool rtc_clear_alarm1(void);
bool rtc_clear_alarm2(void);


// Disable the alarm interrupts
bool rtc_disable_alarm1(void);
bool rtc_disable_alarm2(void);

#endif // RTC_H