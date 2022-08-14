#include <stdint.h>

#ifndef UTILITIES_H_
#define UTILITIES_H_

/* Show the clock value on the display.
 *
 * Mode can be:
 *  HOUR_MIN_SEC  -> __HH MM:SS
 *  DOTW_HOUR_MIN -> DDD_ HH:MM
 *  YEAR_MON_DAY  -> 20YY MM:DD */
void show_time(uint8_t mode);

/* Restore display brightness to the saved setting. */
void restore_display_brightness();

/* Apply new brightness level and show the text "BRIG   N" where N is the 
 * level. */
void apply_display_brightness_level(uint8_t level);

/* Set RTC alarm next [mode].
 *
 * [mode] can be: SECOND, MINUTE or HOUR. */
void set_alarm_next(uint8_t mode);

/* In- or decrement time value on the RTC. Also return the new value. */
uint8_t increment_time_value(uint8_t type);
uint8_t decrement_time_value(uint8_t type);

#endif  // UTILITIES_H_
