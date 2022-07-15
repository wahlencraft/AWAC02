#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "time.h"
#include "io.h"
#include "twi.h"
#include "log.h"
#include "flash.h"

#define ALPHANUMERIC_BUFFER_LEN 13
static uint8_t alphanumeric_buffer[ALPHANUMERIC_BUFFER_LEN];

void clear_alphanumeric_buffer() {
    for (uint8_t i=0; i<13; ++i)
        alphanumeric_buffer[i] = 0;
}

/* Convert an ASCII character to bitmap for the display. Add it to the buffer. */
void add_char_to_alphanumeric_buffer(char c, uint8_t pos) {
    uint16_t segment_map = ascii2seg(c);

    // Convert the standard bitmap to some wierd crap needed for this display.
    //
    // Every segment has an address (0-12) and a bit (0-8).
    // The bit depends on the segment position and the position on the display.
    // The segment index determines if the bit is low (0-3) or high (4-7)
    // for display position 0-3 (left to right).
    //
    // Table explaining the segment maping:
    //  Standard bit  | 0  1  2  3  4  5  6  7  8  9 10 11 12 13
    // ---------------|-----------------------------------------
    //    Address:    | 0  2  4  6  8 10 12  0  2  4  6  8 10 12
    // High/Low digits| L  L  L  L  L  L  L  H  H  H  H  H  H  H
    uint8_t bit_index = 0;
    while (segment_map) {
        uint8_t one = segment_map & 1;
        if (one) {
            //printf("\tone for bit index %d\n", bit_index);
            uint8_t address = (bit_index % 7)*2;
            uint8_t is_high = (bit_index > 6) ? 1 : 0;
            alphanumeric_buffer[address] |= 1 << (pos + 4*is_high);
        }
        segment_map = segment_map >> 1;
        ++bit_index;
    }
}

/* Update the alphanumeric buffer with a new message.
 *
 * msg must be 4 char long.
 * Also return a pointer to the alphanumeric buffer.
 */
uint8_t* update_alphanumeric_buffer(char *msg) {
    clear_alphanumeric_buffer();
    add_char_to_alphanumeric_buffer(msg[0], 0);
    add_char_to_alphanumeric_buffer(msg[1], 1);
    add_char_to_alphanumeric_buffer(msg[2], 2);
    add_char_to_alphanumeric_buffer(msg[3], 3);

    return alphanumeric_buffer;
}

extern uint8_t twi_sla_addr;
extern uint8_t twi_regcom;
extern uint8_t twi_data_len;
extern uint8_t twi_data[16];
extern uint8_t twi_data_ptr;

int main(void){
    init_log(INFO | ERROR | WARNING);

    TWI_init();
    start_counter0();
    start_counter1();
    sei();

    sleep_ms0(1);

    // Test some logging
    log(ERROR, "FATAL ERROR\n");
    log(WARNING, "Warning: %d\n", 4);

    // Initiate display
    log(INFO, "Initiate display\n");
    TWI_write_byte(0x70, 0b00100001);
    TWI_write_byte(0x70, 0b10100011);
    TWI_write_byte(0x70, 0b11100000);
    TWI_write_byte(0x70, 0b10000001);

    // Write to display
    log(INFO, "Write to display\n");

    char c0 = ' ';
    char c1 = c0 + 1;
    char c2 = c0 + 2;
    char c3 = c0 + 3;
    while (c3 != 0x7f) {
        clear_alphanumeric_buffer();
        add_char_to_alphanumeric_buffer(c0, 0);
        add_char_to_alphanumeric_buffer(c1, 1);
        add_char_to_alphanumeric_buffer(c2, 2);
        add_char_to_alphanumeric_buffer(c3, 3);

        TWI_write_bytes(0x70, 0b0, alphanumeric_buffer, 13);
        sleep_ms1(1000);

        c0++;
        c1++;
        c2++;
        c3++;
    }
    clear_alphanumeric_buffer();
    TWI_write_bytes(0x70, 0b0, alphanumeric_buffer, 13);

    log(INFO, "Test program finished\n");

    while (1);

    return 0;
}

