#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

// SSD1306 I²C address for a 128×64 OLED display.
#define SSD1306_ADDR          0x3C

// Control byte definitions.
#define SSD1306_CONTROL_CMD   0x00  // Indicates subsequent bytes are commands.
#define SSD1306_CONTROL_DATA  0x40  // Indicates subsequent bytes are data.

// Function prototypes:

/**
 * @brief Initialize the SSD1306 display.
 *
 * Sends the full initialization sequence in one I²C transaction.
 *
 * @return true if initialization succeeds, false otherwise.
 */
bool display_init(void);

/**
 * @brief Turn the display on.
 *
 * Sends only the "display on" command.
 *
 * @return true on success.
 */
bool display_turn_on(void);

/**
 * @brief Clear the display.
 *
 * Writes zeros to all pixels.
 */
void display_clear(void);

/**
 * @brief Set the display start line.
 *
 * @param start_line Starting line (0–63).
 */
void display_set_start_line(uint_fast8_t start_line);

/**
 * @brief Display text on the screen.
 *
 * Uses an 8×8 pixel font (font8x8_basic_tr) to render characters.
 * Newline characters move to the next page.
 *
 * @param text Pointer to a null-terminated string.
 */
void display_text(const char *text);

#endif // DISPLAY_H
