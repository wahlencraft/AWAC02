#include "utilities.h"
#include "log.h"
#include "rtc.h"
#include "twi.h"
#include "constants.h"
#include "display.h"
#include "helpers.h"
#include "flash.h"
#include "eeprom.h"

#include <stdint.h>

void show_time(uint8_t mode) {
    char display_l[4] = "    ";
    char display_h[4] = "    ";
    char display_long[8] = "        ";
    switch (mode) {
        case HOUR_MIN_SEC:
            log(RTC, "Show HOUR_MIN_SEC\n");

            // Read time
            TWI_read(RTC_SLAVE_ADDRESS, 0, 3);
            TWI_wait();

            add_bcd_digits_to_string(display_long, twi_data[0], 7);
            add_bcd_digits_to_string(display_long, twi_data[1], 5);
            add_bcd_digits_to_string(display_long, twi_data[2], 3);

            // Write
            set_display_buffer_long_string(display_long, 8);
            add_colon_to_display_buffer(1);

            write_to_all_displays();
            break;
        case DOTW_HOUR_MIN:
            log(RTC, "Show DOTW_HOUR_MIN\n");

            // Read time
            TWI_read(RTC_SLAVE_ADDRESS, 1, 3);
            TWI_wait();

            // Set hour and minute
            add_bcd_digits_to_string(display_h, twi_data[0], 3);
            add_bcd_digits_to_string(display_h, twi_data[1], 1);
            set_display_buffer_string(display_h, 1);
            add_colon_to_display_buffer(1);

            // Set day of the week
            uint8_t day_num = twi_data[2] & 0b111;
            add_dotw_to_string(display_l, day_num, 0);
            set_display_buffer_string(display_l, 0);

            write_to_all_displays();
            break;
        case YEAR_MON_DAY:
            log(RTC, "Show YEAR_MON_DAY\n");

            // Read time
            TWI_read(RTC_SLAVE_ADDRESS, 4, 3);
            TWI_wait();

            // First two digits always 20
            display_long[0] = '2';
            display_long[1] = '0';

            // Set year, month and day
            add_bcd_digits_to_string(display_long, twi_data[0], 7);
            add_bcd_digits_to_string(display_long, twi_data[1], 5);
            add_bcd_digits_to_string(display_long, twi_data[2], 3);

            set_display_buffer_long_string(display_long, 8);
            add_colon_to_display_buffer(1);
            write_to_all_displays();
            break;
        default:
            log(ERROR, "Mode %d does not exist\n", mode);
    }
}

void restore_display_brightness() {
    uint8_t level = EEPROM_read(EEPROM_ADDR_DISPLAY_BRIGHTNESS);
    if (EEPROM_UNINITIALIZED == level) {
        level = 0;
        EEPROM_write(EEPROM_ADDR_DISPLAY_BRIGHTNESS, level);
    }
    set_all_display_brightness(level);
}

void apply_display_brightness_level(uint8_t level) {
    set_all_display_brightness(level);
    set_display_buffer_long_string("BRIG ", 5);
    add_char_to_display_buffers(number_to_ascii(level), 7);
    write_to_all_displays();
}

void set_alarm_next(uint8_t mode) {
    uint8_t value = RTC_get(mode);
    switch (mode) {
        case SECOND:
        case MINUTE:
            value = (value + 1) % 60;
            break;
        case HOUR:
            value = (value + 1) % 24;
            break;
        default:
            log(ERROR, "set_alarm_next() unknown mode %d\n", mode);
    }

    RTC_set_alarm(ALARM0, mode, value);
    RTC_enable_alarm(ALARM0);
}

uint8_t change_time_value(uint8_t type, int8_t change) {
    uint8_t new_value;
    switch (type) {
        case SECOND:
        case MINUTE:
            {
                uint8_t value = RTC_get(type);
                new_value = circular_addition(0, 59, value, change);
                break;
            }
        case HOUR:
            {
                uint8_t value = RTC_get(type);
                new_value = circular_addition(0, 23, value, change);
            }
            break;
        case DOTW:
            {
                uint8_t value = RTC_get(type);
                new_value = circular_addition(1, 7, value, change);
            }
            break;
        case DAY:
            {
                uint8_t values[7];
                RTC_get_all(values);
                uint8_t day = values[4];
                uint8_t month = values[5];
                uint8_t year;
                switch (month) {
                    // Jan, Mar, May, July, Aug, Oct, Dec
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                    case 8:
                    case 10:
                    case 12:
                        new_value = circular_addition(1, 31, day, change);
                        break;
                    // Feb
                    case 2:
                        year = values[6];
                        if (year % 4) {
                            // Not divisible by 4
                            new_value = circular_addition(1, 28, day, change);
                        } else {
                            // Divisible by 4
                            new_value = circular_addition(1, 29, day, change);
                        }
                        break;
                    // Apr, June, Sept, Nov
                    case 4:
                    case 6:
                    case 9:
                    case 11:
                        new_value = circular_addition(1, 30, day, change);
                        break;
                    default:
                        new_value = -1;
                        log(ERROR, "change_time_value, invalid month (%d) when changing day\n", month);
                }
                break;
            }
            break;
        case MONTH:
            {
                uint8_t value = RTC_get(type);
                new_value = circular_addition(1, 12, value, change);
            }
            break;
        case YEAR:
            {
                uint8_t value = RTC_get(type);
                new_value = circular_addition(0, 99, value, change);
            }
            break;
        default:
            new_value = -1;
            log(ERROR, "Change value of unknown type=%d\n", type);
    }
    if (type == SECOND)
        RTC_set_second_and_start(new_value);
    else
        RTC_set(type, new_value);
    return new_value;
}

uint8_t increment_time_value(uint8_t type) {
    return change_time_value(type, 1);
}

uint8_t decrement_time_value(uint8_t type) {
    return change_time_value(type, -1);
}

