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
#include "buttons.h"

int main(void){
    init_log(INFO | ERROR | WARNING | CLOCK);

    start_counter0();
    start_counter1();
    sei();
    TWI_init();

    sleep_ms1(1);

    initiate_all_displays();
    init_buttons();
    sleep_ms1(10);

    // Write to display
    log(INFO, "Start test program\n");

    set_display_buffer_long_string("TESTBUTN", 8);
    write_to_all_displays();


    set_display_buffer_long_string("TEST RTC", 8);
    write_to_all_displays();

    sleep_ms1(1000);

    start_clock();
    uint8_t clock_mode = 0;
    while (1) {
        uint8_t pressed_buttons_buffer = extract_pressed_buttons();
        if (pressed_buttons_buffer) {
            for (uint8_t i=0; i<4; ++i) {
                if (pressed_buttons_buffer & (1<<(i+4))) {
                    printf("Button %d pressed\n", i);
                    clock_mode = i;
                }
            }
        }
        show_clock(clock_mode);
        sleep_ms1(100);
    }

    log(INFO, "Test program finished\n");
    while(1);

    return 0;
}

