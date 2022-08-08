/* This is a header declaring the functions defined by assembly rutines in
 * external_interrupts.S. */

#ifndef EXTERNAL_INTERRUPTS_H_H
#define EXTERNAL_INTERRUPTS_H_H

extern void init_external_interrupts();

/* Get the triggered interrupts. Also clear memory of triggered interrupts. */
extern uint8_t extract_triggered_external_interrupts();

#endif  // EXTERNAL_INTERRUPTS_H_H
