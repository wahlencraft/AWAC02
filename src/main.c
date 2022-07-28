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

int main(void){
    init_log(INFO | ERROR | WARNING | TWI);

    start_counter0();
    start_counter1();
    sei();
    TWI_init();

    // Enable debug led
    enable();
    toggle();

    sleep_ms0(1);

    initiate_all_displays();
    sleep_ms0(10);


    // Write to display
    log(INFO, "Start test program\n");

    set_display_buffer_long_string("TEST RTC", 8);
    write_to_all_displays();

    sleep_ms1(1000);

    log(INFO, "Test RTC\n");
    uint8_t config[] = {0x80};
    TWI_write_bytes(0b1101111, 0, config, 1);

    while (1) {
        busy_wait_ms1(1000);
        TWI_read(0b1101111, 0, 3);
        TWI_wait();
        char s0 = (0x0f & twi_data[0]) + 0x30;
        char s1 = ((0x70 & twi_data[0])>>4) + 0x30;
        char m0 = (0x0f & twi_data[1]) + 0x30;
        char m1 = ((0x70 & twi_data[1])>>4) + 0x30;
        char h0 = (0x0f & twi_data[2]) + 0x30;
        char h1 = ((0x70 & twi_data[2])>>4) + 0x30;
        clear_all_display_buffers();
        add_char_to_display_buffer(s0, 3, 1);
        add_char_to_display_buffer(s1, 2, 1);
        add_char_to_display_buffer(m0, 1, 1);
        add_char_to_display_buffer(m1, 0, 1);
        add_colon_to_display_buffer(1);
        add_char_to_display_buffer(h0, 3, 0);
        add_char_to_display_buffer(h1, 2, 0);

        write_to_all_displays();
    }

    log(INFO, "Test program finished\n");
    while(1);

    return 0;
}

