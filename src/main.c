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
#include "clock.h"

int main(void){
    init_log(INFO | ERROR | WARNING | CLOCK);

    start_counter0();
    start_counter1();
    sei();
    TWI_init();

    // Enable debug led
    enable();

    sleep_ms0(1);

    initiate_all_displays();
    sleep_ms0(10);

    // Write to display
    log(INFO, "Start test program\n");

    set_display_buffer_long_string("TEST RTC", 8);
    write_to_all_displays();

    sleep_ms1(1000);

    start_clock();
    while (1) {
        show_clock(HOUR_MIN_SEC);
        //show_clock(DOTW_HOUR_MIN);
        //show_clock(YEAR_MON_DAY);
        sleep_ms1(1000);
    }

    log(INFO, "Test program finished\n");
    while(1);

    return 0;
}

