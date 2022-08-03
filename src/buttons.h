/* This is a header declaring the functions defined by assembly rutines in
 * buttons.S. */

#ifndef BUTTONS_H_
#define BUTTONS_H_

extern void init_buttons();

/* Get the pressed buttons. Also clear pressed button memory. */
extern uint8_t extract_pressed_buttons();

#endif  // BUTTONS_H_
