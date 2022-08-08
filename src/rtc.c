#include "rtc.h"
#include "log.h"
#include "twi.h"
#include "display.h"
#include "flash.h"
#include "helpers.h"

#include <stdint.h>
#include <stdbool.h>

#include "time.h"

#define RTC_SLAVE_ADDRESS 0b1101111
#define RTC_control_reg 0x07
#define RTC_ALARM0_OFFSET 0x0a
#define RTC_ALARM1_OFFSET 0x11

static uint8_t RTC_ALARM_OFFSETS[2] = { RTC_ALARM0_OFFSET, RTC_ALARM1_OFFSET };

void RTC_start() {
    //log(CLOCK, "Start RTC\n");
    TWI_read(RTC_SLAVE_ADDRESS, 0, 1);
    TWI_wait();
    if (twi_data[0] & 0x80) {
        //log(CLOCK, "RTC already running\n");
    } else {
        uint8_t data[1] = { twi_data[0] | 0x80 };
        TWI_write_bytes(RTC_SLAVE_ADDRESS, 0, data, 1);
    }
}

void add_digits_to_string(char *str, uint8_t digits, uint8_t pos) {
    // digits is BCD and should first be converted to ASCII
    str[pos] = '0' + (digits & 0x0f);
    str[pos - 1] = '0' + ((digits & 0x70) >> 4);
}

uint8_t RTC_get(uint8_t address) {
    //log(CLOCK, "Get clock (0x%x)\n", address);
    TWI_read(RTC_SLAVE_ADDRESS, address, 1);
    TWI_wait();
    uint8_t data = bcd_to_number(twi_data[0]);
    return data;
}

void RTC_set(uint8_t address, uint8_t value) {
    //log(CLOCK, "Set clock (0x%x = %d)\n", address, value);

    uint8_t osc_status = 0;
    if (value == 0) {
        // Save OSC enable pin status
        TWI_read(RTC_SLAVE_ADDRESS, 0, 1);
        TWI_wait();
        osc_status = twi_data[0] & 0x80;
    }
    uint8_t data[1] = { osc_status | number_to_bcd(value) };
    TWI_write_bytes(RTC_SLAVE_ADDRESS, address, data, 1);
}

void RTC_show(uint8_t mode) {
    char display_l[4] = "    ";
    char display_h[4] = "    ";
    char display_long[8] = "        ";
    switch (mode) {
        case HOUR_MIN_SEC:
            log(CLOCK, "Show HOUR_MIN_SEC\n");

            // Read time
            TWI_read(RTC_SLAVE_ADDRESS, 0, 3);
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
            TWI_read(RTC_SLAVE_ADDRESS, 1, 3);
            TWI_wait();

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
            TWI_read(RTC_SLAVE_ADDRESS, 4, 3);
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

void RTC_enable_alarm(uint8_t alarm) {
    log(CLOCK, "Enable RTC alarm %d\n", alarm);
    TWI_read(RTC_SLAVE_ADDRESS, RTC_control_reg, 1);
    TWI_wait();
    uint8_t ctrl = twi_data[0];
    ctrl |= 1<<(4 + alarm);
    uint8_t data[1] = { ctrl };
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_control_reg, data, 1);
}

void RTC_disable_alarm(uint8_t alarm) {
    log(CLOCK, "Disable RTC alarm %d\n", alarm);
    TWI_read(RTC_SLAVE_ADDRESS, RTC_control_reg, 1);
    TWI_wait();
    uint8_t ctrl = twi_data[0];
    ctrl &= ~(1<<(4 + alarm));
    uint8_t data[1] = { ctrl };
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_control_reg, data, 1);
}

void RTC_set_alarm(uint8_t alarm, uint8_t address, uint8_t value) {
    log(CLOCK, "Set RTC alarm %d:%x = %d\n", alarm, address, value);

    uint8_t rtc_data[6] = {0, 0, 0, 0, 0, 0};
    rtc_data[address] = number_to_bcd(value);
    rtc_data[3] = address<<4;
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_ALARM0_OFFSET, rtc_data, 6);
}

void RTC_clear_alarm(uint8_t alarm) {
    log(CLOCK, "Clear RTC alarm %d\n", alarm);
    TWI_read(RTC_SLAVE_ADDRESS, RTC_ALARM_OFFSETS[alarm] + 3, 1);
    TWI_wait();
    uint8_t data[1] = { twi_data[0] & ~(1<<3)};
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_ALARM_OFFSETS[alarm] + 3, data, 1);
}
