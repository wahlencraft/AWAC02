#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

#define HOUR_MIN_SEC 0
#define DOTW_HOUR_MIN 1
#define YEAR_MON_DAY 2

/* Start the RTC clock. If clock already running do nothing. */
void start_clock();

/* Show the clock value on the display.
 *
 * Mode can be:
 *  HOUR_MIN_SEC  -> __HH MM:SS
 *  DOTW_HOUR_MIN -> DDD_ HH:MM
 *  YEAR_MON_DAY  -> 20YY MM:DD
 */
void show_clock(uint8_t mode);

#endif  // CLOCK_H_
