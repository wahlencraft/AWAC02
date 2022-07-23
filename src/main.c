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

    set_display_buffer_string("TEST", 0);
    write_to_all_displays();
    sleep_ms1(1000);

    TWI_read(0x70, 0, 1);
    TWI_wait();
    log_twi_status_codes();

    set_display_buffer_string("AWAC", 0);

    write_to_all_displays();
    TWI_wait();

    // Clear twi data buffer
    printf("Clear twi data buffer\n");
    for (uint8_t i=0; i<twi_data_len; ++i)
        twi_data[i] = -1;

    TWI_read(0x70, 0, 4);
    TWI_wait();
    log_twi_status_codes();

    printf("Read from display, one byte at a time:\n\t");
    // Reading in one burst does not work.
    // I don't know why.
    // It works for the RTC though.
    for (uint8_t i=0; i<13; ++i) {
        TWI_read(0x70, i, 1);
        TWI_wait();
        printf("0x%x ", twi_data[0]);
    }
    printf("\n");

    printf("Test RTC\n");
    uint8_t config[] = {0x80};
    TWI_write_bytes(0b1101111, 0, config, 1);

    //for (uint8_t i=0; i<=255; ++i) {
    while (1) {
        sleep_ms1(1000);
        TWI_read(0b1101111, 0, 3);
        TWI_wait();
        printf("RTC: %02x:%02x:%02x\n", twi_data[2], twi_data[1], 0x7f & twi_data[0]);
    }

    log(INFO, "Test program finished\n");
    while(1);

    return 0;
}

