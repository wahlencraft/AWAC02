#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

#define ON 1
#define OFF 0

/* Functions for using sparkfun Qwiic Alphanumeric Display.
 *
 * In constants.h define
 *   DISPLAYS: number of displays (default 1)
 *   DISPLAY_SLAVE_ADDRESS_START: The I2C slave address for the first display
 *                               (default 0x70);
 */

/* Initiate all the displays
 *
 * - Turn on the display driver
 * - Turn on the display
 * - Set display blinking to OFF
 * - Set duty cycle to 1/16
 */
void initiate_display(uint8_t display);
void initiate_all_displays();

/* Setup the system oscillator for the display driver chip. */
void set_display_osc(uint8_t on_off, uint8_t display);
void set_all_display_osc(uint8_t on_off);

/* Setup display
 *
 * blink_set  Frequency
 *     0    |    OFF
 *     1    |   2 Hz
 *     2    |   1 Hz
 *     3    |  0.5 Hz
 */
void set_display(uint8_t on_off, uint8_t blink_set, uint8_t display);
void set_all_displays(uint8_t on_off, uint8_t blink_set);

/* Set the display duty-cycle.
 * duty = (level + 1) / 16
 * 0 <= level <= 15
 */
void set_display_brightness(uint8_t level, uint8_t display);
void set_all_display_brightness(uint8_t level);

void clear_display_buffer(uint8_t display);
void clear_all_display_buffers();

/* Convert an ASCII character to bitmap for the display. Add it to a buffer.
 *
 * Do not call with the same pos multiple times between writes.
 *
 * 0 <= pos < 4
 * 0 <= display < DISPLAYS
 */
void add_char_to_display_buffer(char c, uint8_t pos, uint8_t display);

/* Convert an ASCII character to bitmap for the display. Add it to one of the
 * buffers.
 *
 * Note that pos overflows into the next display.
 *
 * Do not call with the same pos multiple times between writes.
 *
 * 0 <= pos < DISPLAYS*4
 */
void add_char_to_display_buffers(char c, uint8_t pos);

void add_colon_to_display_buffer(uint8_t display);

void add_decimal_to_display_buffer(uint8_t display);

/* Update the display buffer with a new string.
 *
 * The string must be 4 characters long
 */
void set_display_buffer_string(char *str, uint8_t display);

/* Update the display buffers with a new string.
 *
 * The message starts on display 0 and overflows into additional displays.
 *
 * 0 <= len < DISPLAYS*4
 */
void set_display_buffer_long_string(char *str, uint8_t len);

void write_to_display(uint8_t display);
void write_to_all_displays();

#endif  // DISPLAY_H_
