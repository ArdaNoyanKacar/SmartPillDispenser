#include "rtc.h"
#include "nrf_twi_mngr.h"
#include "app_error.h"
#include <string.h>



// Helper functions to convert between binary and BCD
static uint8_t bin2bcd(uint8_t bin)
{
    return ((bin / 10) << 4) | (bin % 10);
}

//Convert BCD format to binary
static uint8_t bcd2bin(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd &0x0F);
}


// Start time instance 
rtc_time_t start_time = {
    .year = 25,
    .month = 3,
    .day = 19,
    .hour = 15,
    .minute= 0,
    .second = 0
};




// Initialize the DS3231 RTC Module
// Return a false if the I2C manager has not been set
bool rtc_init(void)
{
    rtc_set(&start_time);
    return i2c_is_initialized();
}



/* Set the DS3231 time registers
    Registers:
    0x00: Seconds
    0x01: Minutes
    0x02: Hours
    0x03: Day of Week (write a fixed value here  like 1)
    0x04: Date
    0x05: Month
    0x06: Year (offset from 2000)
*/
bool rtc_set(const rtc_time_t *time)
{
    printf("RTC SET CALLED!\n");

    if (i2c_write_reg(DS3132_ADDRESS, DS3231_TIME, bin2bcd(time->second)) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set seconds!\n");
        return false;
    }
    else{
        printf("Seconds set SUCCESS!\n");
    }

    if (i2c_write_reg(DS3132_ADDRESS, DS3231_TIME + 1, bin2bcd(time->minute)) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set minute!\n");
        return false;
    }
    else{
        printf("MInutes set SUCCESS!\n");
    }

    if (i2c_write_reg(DS3132_ADDRESS, DS3231_TIME + 2, bin2bcd(time->hour)) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set hour!\n");
        return false;
    }
    else{
        printf("Hours set SUCCESS!\n");
    }

    if (i2c_write_reg(DS3132_ADDRESS, DS3231_TIME + 3, 0x01) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set day of week!\n");
        return false;
    }
    else{
        printf("Day of weekset SUCCESS!\n");
    }

    if (i2c_write_reg(DS3132_ADDRESS, DS3231_TIME + 4, bin2bcd(time->day)) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set the day!\n");
        return false;
    }
    else{
        printf("Day set SUCCESS!\n");
    }

    if (i2c_write_reg(DS3132_ADDRESS, DS3231_TIME + 5, bin2bcd(time->month)) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set month!\n");
        return false;
    }
    else{
        printf("Month set SUCCESS!\n");
    }

    if (i2c_write_reg(DS3132_ADDRESS, DS3231_TIME + 6, bin2bcd((uint8_t)(time->year - 2000))) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set year!\n");
        return false;
    }
    else{
        printf("Year set SUCCESS!\n");
    }
    
    printf("RTC Setup Complete!\n");
    return true;
}


// Get the RTC time registers and convert them to rtc_time_t
bool rtc_get(rtc_time_t *time)
{
    
    uint8_t data;

    // Second
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_TIME, &data) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read seconds!\n");
        return false;
    }
    time->second = bcd2bin(data & 0x7F);

    // Minute
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_TIME + 1, &data) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read minute!\n");
        return false;
    }
    time->minute = bcd2bin(data & 0x7F);


    // Hour
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_TIME + 2, &data) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read hour!\n");
        return false;
    }
    time->hour = bcd2bin(data & 0x3F); // Based on 24 hour mode

    // Day-of-week is skipped, unnecessary (DS3231_TIME + 3)

    // Day of month
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_TIME + 4, &data) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read day!\n");
        return false;
    }
    time->day= bcd2bin(data & 0x3F);

    // Month
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_TIME + 5, &data) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read month\n");
        return false;
    }
    time->month = bcd2bin(data & 0x1F);

    // Year
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_TIME + 6, &data) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read year!\n");
        return false;
    }
    time->year = bcd2bin(data) + 2000;


    return true;
}




/* Set Alarm1
    The mode parameter selects which fields are compared by clearing or setting the MSB
    MSB clear -> compared
    MSB set-> ignored
*/
bool rtc_set_alarm1(rtc_time_t *time, rtc_alarm1_mode_t mode)
{
    uint8_t sec_reg, min_reg, hour_reg, day_reg;

    switch(mode) {
        case RTC_ALARM1_PER_SECOND:
            sec_reg = 0x80;
            min_reg = 0x80;
            hour_reg = 0x80;
            day_reg = 0x80;
            break;
        case RTC_ALARM1_MATCH_SECONDS:
            sec_reg = bin2bcd(time->second) & 0x7F;
            min_reg = 0x80;
            hour_reg = 0x80;
            day_reg = 0x80;
            break;
        case RTC_ALARM1_MATCH_MINUTES_SECONDS:
            sec_reg = bin2bcd(time->second) & 0x7F; 
            min_reg = bin2bcd(time->minute) & 0x7F;
            hour_reg = 0x80;
            day_reg = 0x80;
            break;
        case RTC_ALARM1_MATCH_HOURS_MINUTES_SECONDS:
            sec_reg = bin2bcd(time->second) & 0x7F; 
            min_reg = bin2bcd(time->minute) & 0x7F;
            hour_reg = bin2bcd(time->hour) & 0x7F;
            day_reg = bin2bcd(time->day) &0x7F;
            break;
        default:
            sec_reg = bin2bcd(time->second) & 0x7F; 
            min_reg = bin2bcd(time->minute) & 0x7F;
            hour_reg = bin2bcd(time->hour) & 0x7F;
            day_reg = 0x80;
            break;
    }

    // Write to Alarm1 Registers
    //

    // Write seconds
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_ALARM1, sec_reg) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set Alarm1 second!\n");
        return false;
    }
    // Write minutes
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_ALARM1 + 1, min_reg) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set Alarm1 minute!\n");
        return false;
    }
    // Write hour
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_ALARM1 + 2, hour_reg) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set Alarm1 hour!\n");
        return false;
    }
    //  Write day
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_ALARM1 + 3, day_reg) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set Alarm1 day!\n");
        return false;
    }

    // Enable Alarm1 Interrupts: set A1IE (bit0) in the control register
    uint8_t ctrl;
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_CONTROL, &ctrl) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read RTC control register!\n");
        return false;
    }

    ctrl |= 0x01; // Set bit0 to enable Alarm1 interrupt
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_CONTROL, ctrl) != NRF_SUCCESS)
    {
        printf("ERROR: Could not enable ALARM1 interrupt!\n");
    }

    return true;
}




/* Set Alarm2
    The mode parameter selects which fields are compared by clearing or setting the MSB
    MSB clear -> compared
    MSB set-> ignored
*/
bool rtc_set_alarm2(rtc_time_t *time, rtc_alarm2_mode_t mode)
{
    uint8_t min_reg, hour_reg, day_reg;

    switch(mode) {
        case RTC_ALARM2_PER_MINUTE:
            min_reg = 0x80;
            hour_reg = 0x80;
            day_reg = 0x80;
            break;
        case RTC_ALARM2_MATCH_MINUTES:
            min_reg = bin2bcd(time->minute) & 0x7F;
            hour_reg = 0x80;
            day_reg = 0x80;
            break;
        case RTC_ALARM2_MATCH_HOURS_MINUTES:
            min_reg = bin2bcd(time->minute) & 0x7F; 
            hour_reg = bin2bcd(time->hour) & 0x7F;
            day_reg = 0x80;
            break;
        case RTC_ALARM2_MATCH_DATE_HOURS_MINUTES:
            min_reg = bin2bcd(time->minute) & 0x7F; 
            hour_reg = bin2bcd(time->hour) & 0x7F;
            day_reg = bin2bcd(time->day) & 0x7F;
            break;
        default:
            min_reg = bin2bcd(time->minute) & 0x7F;
            hour_reg = bin2bcd(time->hour) & 0x7F;
            day_reg = 0x80;
            break;
    }

    // Write to Alarm2 Registers
    //

    // Write minutes
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_ALARM2, min_reg) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set Alarm2 minute!\n");
        return false;
    }
    // Write hour
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_ALARM1 + 1, hour_reg) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set Alarm2 hour!\n");
        return false;
    }
    // Write day
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_ALARM2 + 2, day_reg) != NRF_SUCCESS)
    {
        printf("ERROR: Could not set Alarm2 day!\n");
        return false;
    }
    

    // Enable Alarm2 Interrupts: set A2IE (bit0) in the control register
    uint8_t ctrl;
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_CONTROL, &ctrl) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read RTC control register!\n");
        return false;
    }

    ctrl |= 0x02; // Set bit1 to enable Alarm2 interrupt
    if (i2c_write_reg(DS3132_ADDRESS, DS3231_CONTROL, ctrl) != NRF_SUCCESS)
    {
        printf("ERROR: Could not enable ALARM2 interrupt!\n");
    }

    return true;
}






// Clear ALarm1 flag (bit 0 in the status register)
bool rtc_clear_alarm1(void)
{
    uint8_t status;
    if (i2c_read_reg(DS3132_ADDRESS, DS323_STATUS, &status) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read Alarm1 status register!\n");
        return false;
    }
    status &= ~0x01; // Clears bit0, Alarm1 flag
    return i2c_write_reg(DS3132_ADDRESS, DS323_STATUS, status) == NRF_SUCCESS;
}


// Clears Alarm2 flag (bit1 in the status register)
bool rtc_clear_alarm2(void)
{
    uint8_t status;
    if (i2c_read_reg(DS3132_ADDRESS, DS323_STATUS, &status) != NRF_SUCCESS)
    {
        printf("ERROR: Could not read Alarm1 status register!\n");
        return false;
    }
    status &= ~0x02; // Clears bit1, Alarm2 flag
    return i2c_write_reg(DS3132_ADDRESS, DS323_STATUS, status) == NRF_SUCCESS;
}


// Disable alarm1 interrupt by clearing A1IE (bit0) in the control register
bool rtc_disable_alarm1(void)
{
    uint8_t ctrl;
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_CONTROL, &ctrl) != NRF_SUCCESS)
    {
        printf("ERROR: (ALARM1) Could not read control register!\n");
        return false;
    }
    ctrl &= ~0x01; // Clears bit0, Alarm1 flag
    return i2c_write_reg(DS3132_ADDRESS, DS323_STATUS, ctrl) == NRF_SUCCESS;
}



// Disable alarm2 interrupt by clearing A2IE (bit1) in the control register
bool rtc_disable_alarm2(void)
{
    uint8_t ctrl;
    if (i2c_read_reg(DS3132_ADDRESS, DS3231_CONTROL, &ctrl) != NRF_SUCCESS)
    {
        printf("ERROR: (ALARM2) Could not read control register!\n");
        return false;
    }
    ctrl &= ~0x02; // Clears bit1, Alarm2 flag
    return i2c_write_reg(DS3132_ADDRESS, DS323_STATUS, ctrl) == NRF_SUCCESS;
}