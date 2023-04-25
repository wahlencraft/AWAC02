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
#include "menu.h"
#include "utilities.h"
#include "user_alarm.h"
#include "tests.h"
#include "printing.h"

#include "helpers.h"

#define B_SP (1<<BUTTON_SP)
#define B_R (1<<BUTTON_R)
#define B_L (1<<BUTTON_L)
#define I_RTC (1<<RTC_INT)

extern volatile uint8_t irc_counter;

int main(void) {

    init_printing();
    user_alarm_init();

    run_tests();  // Must be after init_log and user_alarm_init
                  //
    init_log(INFO | ERROR | WARNING | STATE);

    start_counter0();
    start_counter1();
    sei();

    user_alarm_pin_enable();

    TWI_init();

    sleep_ms1(1);

    initiate_all_displays();
    sleep_ms1(10);

    log(INFO, "Start program\n");

    set_display_buffer_long_string("STRTPRGM", 8);
    write_to_all_displays();

    sleep_ms1(1000);

    RTC_start();

    init_external_interrupts();

    restore_display_brightness();

    log_time();

    uint8_t clock_mode = MINUTE;

    // Set alarm now (for test purpose)
    uint8_t time_arr_tmp[7];
    RTC_get_all(time_arr_tmp);

    struct UserAlarm alm;
    alm.dotw = time_arr_tmp[DOTW];
    alm.hour = time_arr_tmp[HOUR];
    alm.minute = time_arr_tmp[MINUTE] + 1;
    alm.status = true;

    user_alarm_add(&alm);

    // To avoid firing twice on the same alarm
    uint8_t last_alarm_minute = -1;

    goto enter_clock_mode;

/*=============================================================================
 * CLOCK MODE
 *===========================================================================*/

enter_clock_mode:
    log(STATE, "Enter clock mode\n");
    clock_mode = MINUTE;
    goto clock_mode_show;

clock_mode_handle_irq:
    {
        // Enter low power mode until next interrupt
        set_alarm_next(clock_mode);
        log(STATE, "Clock mode: handle irq\n");
        TWI_wait();
        sleep_until_interrupt();

        uint8_t interrupts = extract_external_interrupts();
        log(INFO, "interrupt vector = 0x%x\n", interrupts);
        if (interrupts & B_SP)
            goto clock_mode_increment;
        if (interrupts & (B_L | B_R))
            goto enter_menu_mode;
        if (interrupts & (1<<RTC_INT)) {
            goto clock_mode_show;
        }
        goto clock_mode_handle_irq;
    }

clock_mode_show:
    show_time(clock_mode);
    log(STATE, "Clock mode: show\n");

    // Check if alarm should fire
    uint8_t time_arr[7];
    RTC_get_all(time_arr);
    if (time_arr[MINUTE] != last_alarm_minute) {
        last_alarm_minute = -1;
        if (user_alarm_exists(time_arr[DOTW], time_arr[HOUR], time_arr[MINUTE])) {
            last_alarm_minute = time_arr[MINUTE];
            goto alarm_mode;
        }
    }
    goto clock_mode_handle_irq;

clock_mode_increment:
    log(STATE, "Clock mode: increment\n");
    clock_mode = (clock_mode + 1) % 3;
    goto clock_mode_show;

/*=============================================================================
 * ALARM MODE
 *===========================================================================*/

alarm_mode:
    {
        log(STATE, "Enter alarm mode\n");

        user_alarm_pin_enable();
        uint8_t timeout_counter = 0;
        bool exit = false;
        do {
            user_alarm_pin_toggle();
            busy_wait_ms1(USER_ALARM_BEEP_MS);

            // Note: This should idealy have been a sleep state. However that
            // does not seem to work consistently. I don't know why. Using busy
            // wait has the disadvantage of power consumption, but also that
            // the alarm can only be stoped between beeps. This can be
            // mitigated using short beeps so that the user does not notice a
            // too large delay from the button press til the beep stops.

            // Check exit conditions
            if (timeout_counter == USER_ALARM_TIMEOUT) {
                log(INFO, "User alarm timeout\n");
                exit = true;
            }
            if (extract_external_interrupts() & B_SP) {
                log(INFO, "SP button pressed\n");
                exit = true;
            }
            timeout_counter++;
        } while (!exit);
        log(INFO, "Exit alarm mode\n");
        user_alarm_pin_off();
        goto enter_clock_mode;
    }

/*=============================================================================
 * MENU MODE
 *===========================================================================*/

enter_menu_mode:
    RTC_disable_alarm(ALARM0);
    menu();

    goto enter_clock_mode;
}

