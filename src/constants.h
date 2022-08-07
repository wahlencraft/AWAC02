#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Enable log
#define LOG

// Configure display
#define DISPLAYS 2
#define DISPLAY_SLAVE_ADDRESS_START 0x70

// External interrupts (buttons etc)
// Note, must be from PORTD (PCINT16 - PCINT23)
#define BUTTON0 PCINT20
#define BUTTON1 PCINT21
#define BUTTON2 PCINT22
#define BUTTON3 PCINT23
#define RTC_INT PCINT19

#endif  // CONSTANTS_H_

