#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "usart.h"
#include "time.h"
#include "io.h"

int main() {
    stdout = &usart_stdout;
    USART_init();
    printf("\nBegin test program\n");

    start_counter0();
    start_counter1();
    sei();

    uint8_t error_count = 0;
    uint8_t success = 0;

    // Test 8 bit waiting/sleeping
    uint8_t time0_8;
    uint8_t time1_8;
    uint8_t const ms_list_8[] = {1, 2, 4, 5, 8, 10, 15, 21, 30, 32};

    success = 0;
    printf("8 bit tests:\n");

    printf("\tbusy_wait: ");
    for (uint8_t i=0; i<sizeof(ms_list_8)/sizeof(uint8_t); ++i) {
        printf("%u->", ms_list_8[i]);
        time0_8 = read_timer0();
        busy_wait_ms0(ms_list_8[i]);
        time1_8 = read_timer0();
        uint8_t measure = timestamp_to_ms0(time0_8, time1_8);
        if (ms_list_8[i] == measure) {
            success++;
        } else {
            error_count++;
        }
        printf("%u ", measure);
    }
    printf("(%u/%u)\n", success, sizeof(ms_list_8)/sizeof(uint8_t));

    success = 0;
    printf("\tsleep: ");
    for (uint8_t i=0; i<sizeof(ms_list_8)/sizeof(uint8_t); ++i) {
        printf("%u->", ms_list_8[i]);
        time0_8 = read_timer0();
        sleep_ms0(ms_list_8[i]);
        time1_8 = read_timer0();
        uint8_t measure = timestamp_to_ms0(time0_8, time1_8);
        if (ms_list_8[i] == measure) {
            success++;
        } else {
            error_count++;
        }
        printf("%u ", measure);
    }
    printf("(%u/%u)\n", success, sizeof(ms_list_8)/sizeof(uint8_t));

    // Test 16 bit waiting/sleeping
    uint16_t time0_16;
    uint16_t time1_16;
    uint16_t const ms_list_16[] = {1, 2, 4, 8, 30, 500, 2000, 8388};

    success = 0;
    printf("16 bit tests:\n");

    printf("\tbusy_wait: ");
    for (uint8_t i=0; i<sizeof(ms_list_16)/sizeof(uint16_t); ++i) {
        printf("%u->", ms_list_16[i]);
        time0_16 = read_timer1();
        busy_wait_ms1(ms_list_16[i]);
        time1_16 = read_timer1();
        uint16_t measure = timestamp_to_ms1(time0_16, time1_16);
        if (ms_list_16[i] == measure) {
            success++;
        } else {
            error_count++;
        }
        printf("%u ", measure);
    }
    printf("(%u/%u)\n", success, sizeof(ms_list_16)/sizeof(uint16_t));

    success = 0;
    printf("\tsleep: ");
    for (uint8_t i=0; i<sizeof(ms_list_16)/sizeof(uint16_t); ++i) {
        printf("%u->", ms_list_16[i]);
        time0_16 = read_timer1();
        sleep_ms1(ms_list_16[i]);
        time1_16 = read_timer1();
        uint16_t measure = timestamp_to_ms1(time0_16, time1_16);
        if (ms_list_16[i] == measure) {
            success++;
        } else {
            error_count++;
        }
        printf("%u ", measure);
    }
    printf("(%u/%u)\n", success, sizeof(ms_list_16)/sizeof(uint16_t));

    // Summary
    if (!error_count) {
        printf("All tests passed\n");
    } else {
        printf("Failed %u tests\n", error_count);
    }

    // Sweep LED brightness
    enable();
    uint8_t ms = 1;
    uint8_t c = 0;
    while(1) {
        c = 0;
        while(c < 10) {
            toggle();
            busy_wait_ms0(ms);
            toggle();
            busy_wait_ms0(16 - ms);
            c++;
        }
        ms = (ms == 15) ? 1 : ms + 1;
    };
}

