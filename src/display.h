#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

#define ON 1
#define OFF 0

/* Functions for using sparkfun Qwiic Alphanumeric Display.
 *
 * Please define DISPLAYS and DISPLAY_SLAVE_ADDRESS_START in constants.h
 */

/* Initiate all the displays
 *
 * - Turn on the display driver
 * - Turn on the display
 * - Set display blinking to OFF
 * - Set duty cycle to 1/16
 */
void initiate_displays();

/* Setup the system oscillator for the display driver chip. */
void set_display_osc(uint8_t on_off, uint8_t display);

/* Setup display
 *
 * blink_set  Frequency
 *     0    |    OFF
 *     1    |   2 Hz
 *     2    |   1 Hz
 *     3    |  0.5 Hz
 */
void set_display(uint8_t on_off, uint8_t blink_set, uint8_t display);

/* Set the display duty-cycle.
 * duty = (level + 1) / 16
 * 0 <= level <= 15
 */
void set_display_brightness(uint8_t level, uint8_t display);

void clear_display_buffer(uint8_t display);

void clear_display_buffers();

/* Convert an ASCII character to bitmap for the display. Add it to one of the 
 * buffers.
 *
 * 0 <= pos < DISPLAYS*4
 */
void add_char_to_display_buffers(char c, uint8_t pos);

/* Convert an ASCII character to bitmap for the display. Add it to a buffer.
 *
 * 0 <= pos < 4
 * 0 <= display < DISPLAYS
 */
void add_char_to_display_buffer(char c, uint8_t pos, uint8_t display);

/* Update the display buffer with a new message. 
 *
 * The message must be 4 characters long
 */
void update_display_buffer(char *msg, uint8_t display);

/* Update the display buffers with a new message.
 *
 * The message starts on display 0 and overflows into additional displays. 
 *
 * 0 <= len < DISPLAYS*4
 */
void update_display_buffers(char *msg, uint8_t len);

void write_to_display(uint8_t display);

void write_to_displays();

#endif  // DISPLAY_H_
