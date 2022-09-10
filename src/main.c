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


    goto enter_clock_mode;

/*=============================================================================
 * CLOCK MODE
 *===========================================================================*/

enter_clock_mode:
    log(STATE, "Enter clock mode\n");
    clock_mode = MINUTE;
    goto clock_mode_show;

clock_mode_check_interrupts:
    uint8_t interrupts = extract_external_interrupts();
    log(STATE, "Clock mode: check interrupt (0x%x)\n", interrupts);
    if (interrupts & (1<<RTC_INT)) {
        goto clock_mode_show;
    }
    if (interrupts & B_SP)
        goto clock_mode_increment;
    if (interrupts & (B_L | B_R))
        goto enter_menu_mode;
    goto clock_mode_wfi;

clock_mode_show:
    show_time(clock_mode);
    log(STATE, "Clock mode: show\n");
    goto clock_mode_wfi;

clock_mode_increment:
    log(STATE, "Clock mode: increment\n");
    clock_mode = (clock_mode + 1) % 3;
    goto clock_mode_show;

clock_mode_wfi:
    set_alarm_next(clock_mode);
    log(STATE, "Clock mode: WFI\n");
    TWI_wait();
    sleep_until_interrupt();
    goto clock_mode_check_interrupts;

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

            // Sleep for USER_ALARM_BEEP_MS or until external interrupt
            set_timer1_irq_alarm_ms(USER_ALARM_BEEP_MS);
            sleep_until_interrupt();

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

