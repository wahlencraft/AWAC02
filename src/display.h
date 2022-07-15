#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

/* Functions for using sparkfun Qwiic Alphanumeric Display.
 *
 * Please define DISPLAYS and DISPLAY_SLAVE_ADDRESS_START in constants.h
 */

void initiate_displays();

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
