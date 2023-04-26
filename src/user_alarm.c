#include "user_alarm.h"
#include "eeprom.h"
#include "log.h"

#include <stdbool.h>

#define WD 0

enum Comparison { lt, gt, eq };

/* Compare user alarms. Read return value as: alm0 is RESULT alm1, where RESULT
 * is of enum Comparisoin.
 *
 * An earlier alarm is considered less than a
 * later alarm. Consideres week day where WD is the earliest, then
 * Monday-Sunday. */
enum Comparison user_alarm_compare(user_alarm_t *alm0, user_alarm_t *alm1) {
    if (alm0->dotw < alm1->dotw)
        return lt;
    if (alm0->dotw > alm1->dotw)
        return gt;
    if (alm0->hour < alm1->hour)
        return lt;
    if (alm0->hour > alm1->hour)
        return gt;
    if (alm0->minute < alm1->minute)
        return lt;
    if (alm0->minute > alm1->minute)
        return gt;
    return eq;
}

void user_alarm_write(uint8_t index, user_alarm_t *alarm) {
    uint8_t high = (alarm->dotw << 5) | (alarm->hour);
    uint8_t low = (alarm->minute << 2) | (alarm->status << 1);
    EEPROM_write(EEPROM_ADDR_USER_ALARM_START + index*2, high);
    EEPROM_write(EEPROM_ADDR_USER_ALARM_START + index*2 + 1, low);
}

void user_alarm_read(uint8_t index, user_alarm_t *alarm) {
    uint8_t high = EEPROM_read(EEPROM_ADDR_USER_ALARM_START + index*2);
    uint8_t low = EEPROM_read(EEPROM_ADDR_USER_ALARM_START + index*2 + 1);
    alarm->dotw = high >> 5;
    alarm->hour = high & 0b00011111;
    alarm->minute = low >> 2;
    alarm->status = (low & 0b10) >> 1;
}

uint8_t user_alarm_get_len() {
    return EEPROM_read(EEPROM_ADDR_USER_ALARM_LEN);
}

void user_alarm_init() {
    if (user_alarm_get_len() == EEPROM_UNINITIALIZED) {
        EEPROM_write(EEPROM_ADDR_USER_ALARM_LEN, 0);
    }
}

uint8_t user_alarm_add(user_alarm_t *alarm) {
    uint8_t index;
    uint8_t len = user_alarm_get_len();
    if (len > 0) {
        // Find the correct spot to put the image with a modified binary search
        uint8_t high = len - 1;
        uint8_t low = 0;

        user_alarm_t existing_alarm;
        do {
            uint8_t mid = (high + low)/2;
            user_alarm_read(mid, &existing_alarm);
            switch (user_alarm_compare(alarm, &existing_alarm)) {
                case lt:
                    // alarm is earlier than existing_alarm
                    // The index of alarm can be no higer than mid
                    high = mid;
                    break;
                case gt:
                    // alarm is later than existing_alarm
                    // The index of alarm can be no lower than mid + 1
                    low = mid + 1;
                    break;
                case eq:
                    // alarm is the same as existing_alarm. Error.
                    return 1;
            }
        } while (low < high);
        index = low;
        // Shift remaining alarms
        for (int16_t i=len-1; i>=index && i >= 0; --i) {
            user_alarm_t alm;
            user_alarm_read(i, &alm);
            user_alarm_write(i + 1, &alm);
        }
    } else {
        index = 0;
    }

    user_alarm_write(index, alarm);
    EEPROM_write(EEPROM_ADDR_USER_ALARM_LEN, len + 1);
    return 0;
}

uint8_t user_alarm_delete(uint8_t index) {
    uint8_t len = user_alarm_get_len();
    if (index < len && len > 0) {
        user_alarm_t alarm_buffer;

        uint8_t i = index + 1;
        while (i < len) {
            // Move element i to index i - 1
            user_alarm_read(i, &alarm_buffer);
            user_alarm_write(i - 1, &alarm_buffer);
            i++;
        }
        EEPROM_write(EEPROM_ADDR_USER_ALARM_LEN, len - 1);
        return 0;
    } else {
        return 1;
    }
}

bool user_alarm_exists(uint8_t dotw, uint8_t hour, uint8_t minute) {
    uint8_t len = user_alarm_get_len();
    user_alarm_t alarm;
    for (uint8_t i=0; i < len; ++i) {
        user_alarm_read(i, &alarm);
        bool dotw_match = false;
        if (alarm.dotw == WD) {
            // WD should match all days 0-5, that is 0b0XX or 0b10X
            dotw_match = (~dotw & 0b100) || (~dotw & 0b010);
        } else {
            dotw_match = alarm.dotw && dotw;
        }
        if (dotw_match) {
            if (hour == alarm.hour && minute == alarm.minute && alarm.status) {
                return true;
            }
        }
    }
    return false;
}

bool user_alarm_exists_exact(uint8_t dotw, uint8_t hour, uint8_t minute) {
    uint8_t len = user_alarm_get_len();
    user_alarm_t alarm;
    for (uint8_t i=0; i < len; ++i) {
        user_alarm_read(i, &alarm);
        if (dotw == alarm.dotw && hour == alarm.hour && minute == alarm.minute) {
            return true;
        }
    }
    return false;
}

