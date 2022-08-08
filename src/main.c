#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "time.h"
#include "io.h"
#include "twi.h"
#include "log.h"
#include "flash.h"
#include "display.h"
#include "rtc.h"
#include "external_interrupts.h"
#include "eeprom.h"

#include "helpers.h"

/* Set RTC alarm next [mode].
 *
 * [mode] can be: SECOND, MINUTE, HOUR or DAY. */
void set_alarm_next(uint8_t mode) {
    uint8_t value = RTC_get(mode);
    value = (value + 1) % 60;

    RTC_set_alarm(ALARM0, mode, value);
    RTC_enable_alarm(ALARM0);
}

int main(void){
    init_log(INFO | ERROR | WARNING | CLOCK);

    start_counter0();
    start_counter1();
    sei();

    enable();

    TWI_init();

    sleep_ms1(1);

    initiate_all_displays();
    sleep_ms1(10);

    log(INFO, "Start test program\n");

    set_display_buffer_long_string("TEST RTC", 8);
    write_to_all_displays();

    sleep_ms1(1000);

    RTC_start();

    init_external_interrupts();

    RTC_set(HOUR, 23);
    RTC_set(MINUTE, 59);

    set_alarm_next(SECOND);

    uint8_t clock_mode = 0;
    while (1) {
        uint8_t ext_interrupts = extract_triggered_external_interrupts();
        switch (ext_interrupts) {
            case 0:
                // Nothing has been pressed
                break;
            case 1<<BUTTON0:
                log(INFO, "Button 0 has been pressed\n");
                clock_mode = 0;
                RTC_show(clock_mode);

                set_alarm_next(SECOND);
                break;
            case 1<<BUTTON1:
                log(INFO, "Button 1 has been pressed\n");
                clock_mode = 1;
                RTC_show(clock_mode);

                set_alarm_next(MINUTE);
                break;
            case 1<<BUTTON2:
                log(INFO, "Button 2 has been pressed\n");
                clock_mode = 2;
                RTC_show(clock_mode);

                set_alarm_next(DAY);
                break;
            case 1<<BUTTON3:
                log(INFO, "Button 3 has been pressed\n");
                clock_mode = 3;
                RTC_show(clock_mode);
                break;
            case 1<<RTC_INT:
                log(INFO, "RTC interrupt received\n");

                RTC_disable_alarm(ALARM0);
                RTC_show(clock_mode);
                if (clock_mode != 3)
                    set_alarm_next(clock_mode);
                break;
            default:
                log(WARNING, "Unknown external interrupt: 0x%x\n", ext_interrupts);
        }
    }

    log(INFO, "Test program finished\n");
    while(1);

    return 0;
}

