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

    set_display_buffer_string("TEST", 0);
    print_all_display_buffers();
    write_to_all_displays();

    sleep_ms1(1000);

    set_display_buffer_long_string("Albin W", 7);

    write_to_all_displays();

    log(INFO, "Test program finished\n");
    while(1);

    return 0;
}

