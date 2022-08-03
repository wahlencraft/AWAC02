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

#define BUTTON_BIT_POSITIONS (1<<PCINT23) | (1<<PCINT22) | (1<<PCINT21) | (1<<PCINT20)
#define BUTTON_MASK BUTTON_BIT_POSITIONS
#define COUNT_MARIGIN 255

static volatile uint8_t pressed_buttons = 0;
static volatile uint8_t pin_memory = 0;
static volatile uint8_t button_filter = 0;

ISR(PCINT2_vect) {
    // Calculate pressed buttons
    uint8_t pins = PIND;
    pins &= BUTTON_MASK;
    uint8_t toggled_pins = pins^pin_memory;
    pin_memory = pins;
    pressed_buttons = (~pins) & toggled_pins;
}

int main(void){
    init_log(INFO | ERROR | WARNING | CLOCK);

    start_counter0();
    start_counter1();
    sei();
    TWI_init();

    sleep_ms1(1);

    initiate_all_displays();
    sleep_ms1(10);

    // Write to display
    log(INFO, "Start test program\n");

    set_display_buffer_long_string("TESTBUTN", 8);
    write_to_all_displays();

    PCICR = 1<<PCIE2;
    PCMSK2 = BUTTON_MASK;
    pin_memory = PIND;
    pin_memory &= BUTTON_MASK;

    uint8_t counters[4] = {0, 0, 0, 0};
    while (1) {
        cli();
        uint8_t pressed_buttons_buffer = pressed_buttons;
        pressed_buttons = 0;
        sei();
        if (pressed_buttons_buffer) {
            for (uint8_t i=0; i<4; ++i) {
                if (pressed_buttons_buffer & (1<<(i+4))) {
                    printf("Button %d pressed\n", i);
                    counters[i] = (counters[i] + 1) % 100;
                }
            }

            clear_all_display_buffers();
            // counter 0
            add_char_to_display_buffers(counters[0] % 10 + '0', 1);
            add_char_to_display_buffers((counters[0] - (counters[0] % 10))/10 + '0', 0);
            // counter 1
            add_char_to_display_buffers(counters[1] % 10 + '0', 3);
            add_char_to_display_buffers((counters[1] - (counters[1] % 10))/10 + '0', 2);
            // counter 2
            add_char_to_display_buffers(counters[2] % 10 + '0', 5);
            add_char_to_display_buffers((counters[2] - (counters[2] % 10))/10 + '0', 4);
            // counter 3
            add_char_to_display_buffers(counters[3] % 10 + '0', 7);
            add_char_to_display_buffers((counters[3] - (counters[3] % 10))/10 + '0', 6);

            add_colon_to_display_buffer(0);
            add_colon_to_display_buffer(1);
            write_to_all_displays();
            TWI_wait();
        }
    }

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

