#include "display.h"
#include "i2c_driver.h"
#include "nrf_error.h"
#include <string.h>
#include <stdio.h>
#include "font8x8_basic.h"  // This header should define font8x8_basic_tr

// SSD1306 initialization command sequence for a 128×64 display.
static const uint8_t ssd1306_init_cmds[] = {
    0xAE,             // Display off
    0xD5, 0x80,       // Set display clock divide ratio/oscillator frequency
    0xA8, 0x3F,       // Set multiplex ratio (1 to 64)
    0xD3, 0x00,       // Set display offset
    0x40,             // Set start line address
    0x8D, 0x14,       // Enable charge pump
    0x20, 0x00,       // Set memory addressing mode to horizontal addressing
    0xA1,             // Set segment re-map (column 127 mapped to SEG0)
    0xC8,             // Set COM output scan direction (remapped)
    0xDA, 0x12,       // Set COM pins hardware configuration
    0x81, 0xCF,       // Set contrast control
    0xD9, 0xF1,       // Set pre-charge period
    0xDB, 0x40,       // Set VCOMH deselect level
    0xA4,             // Resume display from RAM content (output follows RAM)
    0xA6,             // Normal display (non-inverted)
    0xAF              // Display on
};

/*
 * display_write_block
 *
 * Write a block of bytes to the display.
 * It builds a temporary buffer consisting of the control byte (either command or data)
 * followed by the data, and sends it in one I²C transaction.
 */
static ret_code_t display_write_block(uint8_t control, const uint8_t *data, uint8_t length)
{
    uint8_t buf[length + 1];
    buf[0] = control;
    memcpy(&buf[1], data, length);
    return i2c_write_block(SSD1306_ADDR, buf, length + 1);
}

/*
 * display_init
 *
 * Initialize the SSD1306 display by sending the full initialization sequence.
 * 
 */
bool display_init(void)
{
    uint8_t buf[sizeof(ssd1306_init_cmds) + 1];
    buf[0] = SSD1306_CONTROL_CMD;
    memcpy(&buf[1], ssd1306_init_cmds, sizeof(ssd1306_init_cmds));

    ret_code_t err = i2c_write_block(SSD1306_ADDR, buf, sizeof(buf));
    if (err != NRF_SUCCESS) {
        printf("ERROR: Display initialization failed! Err=0x%08X\n", err);
        return false;
    }
    printf("SUCCESS: Display initialized successfully!\n");
    return true;
}

/*
 * display_turn_on
 *
 * Send only the "display on" command (0xAF).
 */
bool display_turn_on(void)
{
    uint8_t cmd = 0xAF;
    ret_code_t err = i2c_write_block(SSD1306_ADDR, &cmd, 1);
    if (err != NRF_SUCCESS) {
        printf("ERROR: Failed to turn on display! Err=0x%08X\n", err);
        return false;
    }
    printf("SUCCESS: Display turned on.\n");
    return true;
}

/*
 * display_set_start_line
 *
 * Sets the display start line by combining 0x40 with the start line.
 */
void display_set_start_line(uint_fast8_t start_line)
{
    if (start_line <= 63) {
        uint8_t cmd = 0x40 | start_line;
        display_write_block(SSD1306_CONTROL_CMD, &cmd, 1);
    }
}

/*
 * display_reset_addressing
 *
 * Resets column and page addressing.
 * 
 */
static void display_reset_addressing(uint8_t page)
{
    uint8_t cmds[] = {
        0x00,       // Lower column address
        0x10,       // Higher column address
        0xB0 | page // Page address (0-7)
    };
    ret_code_t err = display_write_block(SSD1306_CONTROL_CMD, cmds, sizeof(cmds));
    if (err != NRF_SUCCESS) {
        printf("ERROR: Reset display addressing failed!\n");
    } else {
        //printf("SUCCESS: Display addressing reset successfully\n");
    }
}

/*
 * display_clear
 *
 * Clears the display by writing zeros to every pixel.
 */
void display_clear(void)
{
    uint8_t zeros[128];
    memset(zeros, 0, sizeof(zeros));

    for (uint8_t page = 0; page < 8; page++) {
        display_reset_addressing(page);
        display_write_block(SSD1306_CONTROL_DATA, zeros, sizeof(zeros));
    }
}

/*
 * display_text
 *
 * Displays text on the screen using an 8×8 font.
 * Each character is drawn as 8 bytes from the font array.
 * Newline characters cause the text to advance to the next page.
 */
void display_text(const char *text)
{
    if (!text) {
        return;
    }

    size_t text_len = strlen(text);
    uint8_t page = 0;
    display_reset_addressing(page);

    for (size_t i = 0; i < text_len; i++) {
        if (text[i] == '\n') {
            page++;
            if (page >= 8)
                break;
            display_reset_addressing(page);
        } else {
            // Each character is rendered using an 8-byte bitmap.
            display_write_block(SSD1306_CONTROL_DATA, (uint8_t *)font8x8_basic_tr[(uint8_t)text[i]], 8);
        }
    }
}