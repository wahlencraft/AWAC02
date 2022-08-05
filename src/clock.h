#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

#define HOUR_MIN_SEC 0
#define DOTW_HOUR_MIN 1
#define YEAR_MON_DAY 2

/* Start the RTC clock. If clock already running do nothing. */
void start_clock();

/* Get the clock value at address.
 *
 * Address | Description     | Range
 * --------|-----------------|------
 *    0    | Seconds         | 0-59
 *    1    | Minutes         | 0-59
 *    2    | Hours           | 0-23
 *    3    | Day of the week | 1-7
 *    4    | Day (date)      | 1-31
 *    5    | Month           | 1-12
 *    6    | Year            | 0-99
 */
uint8_t get_clock(uint8_t address);

/* Set the clock value at address.
 *
 * Address | Description     | Range
 * --------|-----------------|------
 *    0    | Seconds         | 0-59
 *    1    | Minutes         | 0-59
 *    2    | Hours           | 0-23
 *    3    | Day of the week | 1-7
 *    4    | Day (date)      | 1-31
 *    5    | Month           | 1-12
 *    6    | Year            | 0-99
 */
void set_clock(uint8_t address, uint8_t value);

/* Show the clock value on the display.
 *
 * Mode can be:
 *  HOUR_MIN_SEC  -> __HH MM:SS
 *  DOTW_HOUR_MIN -> DDD_ HH:MM
 *  YEAR_MON_DAY  -> 20YY MM:DD
 */
void show_clock(uint8_t mode);

#endif  // CLOCK_H_
