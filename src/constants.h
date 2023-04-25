#include <avr/io.h>

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define LOG  // Enable log
#define RUN_TESTS  // Enable tests

#define USER_ALARM_BEEP_MS 400
#define USER_ALARM_TIMEOUT 80 // Number of beeps*2 before alarm timeout. Range 1-255.

// Configure display
#define DISPLAYS 2
#define DISPLAY_SLAVE_ADDRESS_START 0x70

// External interrupts (buttons etc)
// Note, must be from PORTD (PCINT16 - PCINT23)
#define BUTTON_SP PCINT23
#define BUTTON_L PCINT22
#define BUTTON_R PCINT21
#define RTC_INT PCINT20

// RTC
#define RTC_SLAVE_ADDRESS 0b1101111
#define RTC_control_reg 0x07
#define RTC_ALARM0_OFFSET 0x0a
#define RTC_ALARM1_OFFSET 0x11

// PORT C
#define USER_ALARM_PIN 0

#endif  // CONSTANTS_H_

