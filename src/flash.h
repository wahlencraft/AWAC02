/* This is a header declaring the functions defined by assembly rutines in
 * flash.S. */

#ifndef FLASH_H_
#define FLASH_H_

/* Convert an ASCII character to the bitmap for a 14 segment digit */
extern uint16_t ascii2seg(char c);

#endif  // FLASH_H_
