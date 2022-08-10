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
#include "sleep.h"

#include "helpers.h"

extern volatile uint8_t irc_counter;

/* Set RTC alarm next [mode].
 *
 * [mode] can be: SECOND, MINUTE or HOUR. */
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

int main(void) {
    init_log(INFO | ERROR | WARNING | RTC);

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

    uint8_t clock_mode = MINUTE;

    goto enter_clock_mode;

/*=============================================================================
 * CLOCK MODE
 *===========================================================================*/

enter_clock_mode:
    log(INFO, "Enter clock mode\n");
    clock_mode = MINUTE;
    goto clock_mode_show;

clock_mode_check_interrupts:
    uint8_t interrupts = extract_external_interrupts();
    log(INFO, "Clock mode: check interrupt (0x%x)\n", interrupts);
    if (interrupts & (1<<RTC_INT)) {
        //RTC_clear_alarm(ALARM0);
        goto clock_mode_show;
    }
    if (interrupts & (1<<BUTTON_SP))
        goto clock_mode_increment;
    if (interrupts & ((1<<BUTTON_L) | (1<<BUTTON_R)))
        goto enter_menu_mode;
    goto clock_mode_wfi;

clock_mode_show:
    RTC_show(clock_mode);
    log(INFO, "Clock mode: show\n");
    goto clock_mode_wfi;

clock_mode_increment:
    log(INFO, "Clock mode: increment\n");
    clock_mode = (clock_mode + 1) % 3;
    goto clock_mode_show;

clock_mode_wfi:
    set_alarm_next(clock_mode);
    log(INFO, "Clock mode: WFI\n");
    TWI_wait();
    sleep_until_interrupt();
    goto clock_mode_check_interrupts;

/*=============================================================================
 * MENU MODE
 *===========================================================================*/

enter_menu_mode:
    set_display_buffer_long_string("MENUTODO", 8);
    write_to_all_displays();
    log(INFO, "Enter menu mode\n");

    while (1);
}

