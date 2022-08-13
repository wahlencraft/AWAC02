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
