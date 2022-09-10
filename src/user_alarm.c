#include "user_alarm.h"
#include "eeprom.h"
#include "log.h"

#include <stdbool.h>

#define WD 0

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

void user_alarm_add(user_alarm_t *alarm) {
    uint8_t len = user_alarm_get_len();
    user_alarm_write(len, alarm);
    EEPROM_write(EEPROM_ADDR_USER_ALARM_LEN, len + 1);
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

