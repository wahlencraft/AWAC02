/* This is a header declaring the functions defined by assembly rutines in
 * io.S. */

#ifndef IO_H_H
#define IO_H_H

/* The user alarm pin is the pin used for user-defined alarms. It is connected
 * to the alarm buzzer and the alarm led. */

extern void user_alarm_pin_enable();
extern void user_alarm_pin_toggle();
extern void user_alarm_pin_off();

#endif  // IO_H_H

