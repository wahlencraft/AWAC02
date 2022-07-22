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

    TWI_init();
    start_counter0();
    start_counter1();
    sei();

    // Enable debug led
    enable();

    sleep_ms0(1);

    initiate_all_displays();
    sleep_ms0(10);

    // Write to display
    log(INFO, "Start test program\n");

    char c0 = ' ';
    char c1 = c0 + 1;
    char c2 = c0 + 2;
    char c3 = c0 + 3;

    set_display_buffer_string("TEST", 0);
    write_to_all_displays();

    uint8_t data = TWI_read_byte(0x70, 0);
    printf("Data read from display: 0x%x\n", data);

    set_display_buffer_string("AWAC", 0);
    write_to_all_displays();

    data = TWI_read_byte(0x70, 0);
    printf("Data read from display: 0x%x\n", data);

    log_twi_status_codes();

    log(INFO, "Test program finished\n");
    while(1);

    while (1) {
        clear_all_display_buffers();
        add_char_to_display_buffers(c0, 0);
        add_char_to_display_buffers(c1, 1);
        add_char_to_display_buffers(c2, 2);
        add_char_to_display_buffers(c3, 3);

        write_to_all_displays();
        //sleep_ms1(1000);  // Only needed when not logging

        c0 = (c0 == 0x7f) ? ' ' : c0 + 1;
        c1 = (c1 == 0x7f) ? ' ' : c1 + 1;
        c2 = (c2 == 0x7f) ? ' ' : c2 + 1;
        c3 = (c3 == 0x7f) ? ' ' : c3 + 1;
    }
    clear_all_display_buffers();
    write_to_all_displays();

    log(INFO, "Test program finished\n");

    while (1);

    return 0;
}

