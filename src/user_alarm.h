#ifndef USER_ALARM_H_
#define USER_ALARM_H_

#include <stdbool.h>
#include <stdint.h>

/* This file declares the functions used to handle user alarms. A user alarm is
 * a time when the buzzer should start beeping. A user alarm should not be
 * confused with a RTC alarm which is much more common.
 */

typedef struct UserAlarm {
    uint8_t dotw;
    uint8_t hour;
    uint8_t minute;
    bool status;
} user_alarm_t;

/* Find which alarm is earliest.
 *
 * Alarms are sorted with DOTW=WD being first.
 * Returns 0 if alm0 is first, 1 if alm1 is first and -1 if they are the same. */
int8_t user_alarm_find_first(user_alarm_t *alm0, user_alarm_t *alm1);

/* Writes an user alarm to EEPROM directly with index.
 *
 * Warning this is very slow. Avoid using if possible. */
void user_alarm_write(uint8_t index, user_alarm_t *alarm);

/* Read an user alarm from EEPROM directly with index. */
void user_alarm_read(uint8_t index, user_alarm_t *alarm);

/* Get the number of user alarms stored in EEPROM. */
uint8_t user_alarm_get_len();

void user_alarm_init();

/* Add a new user alarm, return 0 on success*/
uint8_t user_alarm_add(user_alarm_t *alarm);

/* Deletes an alarm, return 0 on success. */
uint8_t user_alarm_delete(uint8_t index);

/* Check if active alarm exitsts.
 *
 * dotw = MON-FRI also matches if there is an alarm with dotw=WD.
 * Only returns true if the found alarm is active.
 */
bool user_alarm_exists(uint8_t dotw, uint8_t hour, uint8_t minute);

/* Check if alarm exists matching exactly.
 *
 * dotw only matches the same day.
 * Does not check the active status of the alarm.
 */
bool user_alarm_exists_exact(uint8_t dotw, uint8_t hour, uint8_t minute);

void user_alarm_tests();

#endif  // USER_ALARM_H_
