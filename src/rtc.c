#include "rtc.h"
#include "log.h"
#include "twi.h"
#include "display.h"
#include "flash.h"
#include "helpers.h"

#include <stdint.h>
#include <stdbool.h>

#include "time.h"

static uint8_t RTC_ALARM_OFFSETS[2] = { RTC_ALARM0_OFFSET, RTC_ALARM1_OFFSET };

void RTC_start() {
    log(RTC, "Start RTC\n");
    TWI_read(RTC_SLAVE_ADDRESS, 0, 1);
    TWI_wait();
    if (twi_data[0] & 0x80) {
        log(RTC, "RTC already running\n");
    } else {
        uint8_t data[1] = { twi_data[0] | 0x80 };
        TWI_write_bytes(RTC_SLAVE_ADDRESS, 0, data, 1);
    }
}

uint8_t RTC_get(uint8_t address) {
    log(RTC, "Get clock (0x%x)\n", address);
    TWI_read(RTC_SLAVE_ADDRESS, address, 1);
    TWI_wait();
    uint8_t data = bcd_to_number(twi_data[0]);
    return data;
}

void RTC_set(uint8_t address, uint8_t value) {
    log(RTC, "Set clock (0x%x = %d)\n", address, value);

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

void RTC_enable_alarm(uint8_t alarm) {
    log(RTC, "Enable RTC alarm %d\n", alarm);
    TWI_read(RTC_SLAVE_ADDRESS, RTC_control_reg, 1);
    TWI_wait();
    uint8_t ctrl = twi_data[0];
    ctrl |= 1<<(4 + alarm);
    uint8_t data[1] = { ctrl };
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_control_reg, data, 1);
}

void RTC_disable_alarm(uint8_t alarm) {
    log(RTC, "Disable RTC alarm %d\n", alarm);
    TWI_read(RTC_SLAVE_ADDRESS, RTC_control_reg, 1);
    TWI_wait();
    uint8_t ctrl = twi_data[0];
    ctrl &= ~(1<<(4 + alarm));
    uint8_t data[1] = { ctrl };
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_control_reg, data, 1);
}

void RTC_set_alarm(uint8_t alarm, uint8_t address, uint8_t value) {
    log(RTC, "Set RTC alarm %d:%x = %d\n", alarm, address, value);

    uint8_t rtc_data[6] = {0, 0, 0, 0, 0, 0};
    rtc_data[address] = number_to_bcd(value);
    rtc_data[3] = address<<4;
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_ALARM0_OFFSET, rtc_data, 6);
}

void RTC_clear_alarm(uint8_t alarm) {
    log(RTC, "Clear RTC alarm %d\n", alarm);
    TWI_read(RTC_SLAVE_ADDRESS, RTC_ALARM_OFFSETS[alarm] + 3, 1);
    TWI_wait();
    uint8_t data[1] = { twi_data[0] & ~(1<<3)};
    TWI_write_bytes(RTC_SLAVE_ADDRESS, RTC_ALARM_OFFSETS[alarm] + 3, data, 1);
}
