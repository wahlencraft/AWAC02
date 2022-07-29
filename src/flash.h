/* This is a header declaring the functions defined by assembly rutines in
 * flash.S. */

#ifndef FLASH_H_
#define FLASH_H_

/* Convert an ASCII character to the bitmap for a 14 segment digit */
extern uint16_t ascii2seg(char c);

/* Convert a day number (1-7) to a 3 letter string and put it in str. */
extern void day_num_2_name(char *str, uint8_t num, uint8_t pos);

#endif  // FLASH_H_
