#include "clock.h"
#include "log.h"
#include "twi.h"
#include "display.h"
#include "flash.h"

#define RTC_slave_address 0b1101111

void start_clock() {
    log(CLOCK, "Start RTC\n");
    TWI_read(RTC_slave_address, 0, 1);
    TWI_wait();
    if (twi_data[0] & 0x80) {
        log(CLOCK, "RTC already running\n");
    } else {
        uint8_t data[1] = { twi_data[0] | 0x80 };
        TWI_write_bytes(RTC_slave_address, 0, data, 1);
    }
}

void add_digits_to_string(char *str, uint8_t digits, uint8_t pos) {
    // digits is BCD and should first be converted to ASCII
    str[pos] = '0' + (digits & 0x0f);
    str[pos - 1] = '0' + ((digits & 0x70) >> 4);
}

void show_clock(uint8_t mode) {
    char display_l[4] = "    ";
    char display_h[4] = "    ";
    char display_long[8] = "        ";
    switch (mode) {
        case HOUR_MIN_SEC:
            log(CLOCK, "Show HOUR_MIN_SEC\n");

            // Read time
            TWI_read(RTC_slave_address, 0, 3);
            TWI_wait();

            add_digits_to_string(display_long, twi_data[0], 7);
            add_digits_to_string(display_long, twi_data[1], 5);
            add_digits_to_string(display_long, twi_data[2], 3);

            // Write
            set_display_buffer_long_string(display_long, 8);
            add_colon_to_display_buffer(1);

            write_to_all_displays();
            break;
        case DOTW_HOUR_MIN:
            log(CLOCK, "Show DOTW_HOUR_MIN\n");

            // Read time
            TWI_read(RTC_slave_address, 1, 3);
            TWI_wait();

            printf("Raw %x:%x:%x\n", twi_data[2], twi_data[1], twi_data[0]);
            printf("Mask %x:%x:%x\n", twi_data[2] & 0b111, twi_data[1], twi_data[0]);

            // Set hour and minute
            add_digits_to_string(display_h, twi_data[0], 3);
            add_digits_to_string(display_h, twi_data[1], 1);
            set_display_buffer_string(display_h, 1);
            add_colon_to_display_buffer(1);

            // Set day of the week
            uint8_t day_num = twi_data[2] & 0b111;
            day_num_2_name(display_l, day_num, 0);
            set_display_buffer_string(display_l, 0);

            write_to_all_displays();
            break;
        case YEAR_MON_DAY:
            log(CLOCK, "Show YEAR_MON_DAY\n");

            // Read time
            TWI_read(RTC_slave_address, 4, 3);
            TWI_wait();

            // First two digits always 20
            display_long[0] = '2';
            display_long[1] = '0';

            // Set year, month and day
            add_digits_to_string(display_long, twi_data[0], 7);
            add_digits_to_string(display_long, twi_data[1], 5);
            add_digits_to_string(display_long, twi_data[2], 3);

            set_display_buffer_long_string(display_long, 8);
            add_colon_to_display_buffer(1);
            write_to_all_displays();
            break;
        default:
            log(ERROR, "Mode %d does not exist\n", mode);
    }
}
