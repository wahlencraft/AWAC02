#ifndef RTC_H_
#define RTC_H_H

#include <stdint.h>
#include <stdbool.h>

#define HOUR_MIN_SEC 0
#define DOTW_HOUR_MIN 1
#define YEAR_MON_DAY 2

// Addresses
#define SECOND 0
#define MINUTE 1
#define HOUR 2
#define DOTW 3
#define DAY 4
#define MONTH 5
#define YEAR 6

#define ALARM0 0
#define ALARM1 1

/* Start the RTC clock. If clock already running do nothing. */
void RTC_start();

/* Get the clock value at address.
 *
 * Address                | Range
 * -----------------------|------
 * SECOND                 | 0-59
 * MINUTE                 | 0-59 * HOUR                   | 0-23
 * DOTW (day of the week) | 1-7
 * DAY (date)             | 1-31
 * MONTH                  | 1-12
 * YEAR                   | 0-99
 */
uint8_t RTC_get(uint8_t address);

/* Set the clock value at address.
 *
 * Address                | Range
 * -----------------------|------
 * SECOND                 | 0-59
 * MINUTE                 | 0-59
 * HOUR                   | 0-23
 * DOTW (day of the week) | 1-7
 * DAY (date)             | 1-31
 * MONTH                  | 1-12
 * YEAR                   | 0-99
 */
void RTC_set(uint8_t address, uint8_t value);

/* Enable ALARM0 or ALARM1 */
void RTC_enable_alarm(uint8_t alarm);

/* Disable ALARM0 or ALARM1 */
void RTC_disable_alarm(uint8_t alarm);

/* Set one alarm value.
 *
 * alarm = ALARM0 or ALARM1,
 * address = SECOND, MINUTE, HOUR, DOTW, DAY.
 * value depends on address. */
void RTC_set_alarm(uint8_t alarm, uint8_t address, uint8_t value);

/* Clear ALARM0 or ALARM1. Not needed if RTC_set_alarm is called instead.*/
void RTC_clear_alarm(uint8_t alarm);

#endif  // RTC_H_H
