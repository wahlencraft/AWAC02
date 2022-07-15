#include "display.h"
#include "flash.h"
#include "twi.h"
#include "constants.h"

#define DISPLAY_BUFFER_LEN 13

static uint8_t display_buffer[DISPLAYS][DISPLAY_BUFFER_LEN];

void initiate_displays() {
    for (uint8_t i=0; i<DISPLAYS; ++i) {
        TWI_write_byte(DISPLAY_SLAVE_ADDRESS_START + i, 0b00100001);
        TWI_write_byte(DISPLAY_SLAVE_ADDRESS_START + i, 0b10100011);
        TWI_write_byte(DISPLAY_SLAVE_ADDRESS_START + i, 0b11100000);
        TWI_write_byte(DISPLAY_SLAVE_ADDRESS_START + i, 0b10000001);
    }
}


void clear_display_buffer(uint8_t display) {
    for (uint8_t i=0; i<13; ++i)
        display_buffer[display][i] = 0;
}

void clear_display_buffers() {
    for (uint8_t i=0; i<DISPLAYS; ++i)
        for (uint8_t j=0; j<13; ++j)
            display_buffer[i][j] = 0;
}

void add_char_to_display_buffer(char c, uint8_t pos, uint8_t display) {
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
            uint8_t address = (bit_index % 7)*2;
            uint8_t is_high = (bit_index > 6) ? 1 : 0;
            display_buffer[display][address] |= 1 << (pos + 4*is_high);
        }
        segment_map = segment_map >> 1;
        ++bit_index;
    }
}

void add_char_to_display_buffers(char c, uint8_t pos) {
    add_char_to_display_buffer(c, pos%4, pos/4);
}

void update_display_buffer(char *msg, uint8_t display) {
    clear_display_buffer(display);
    add_char_to_display_buffer(msg[0], 0, display);
    add_char_to_display_buffer(msg[1], 1, display);
    add_char_to_display_buffer(msg[2], 2, display);
    add_char_to_display_buffer(msg[3], 3, display);
}

void update_display_buffers(char *msg, uint8_t len) {
    for (uint8_t i=0; i < (len - 1)/4 + 1; ++i) {
        clear_display_buffer(i);
    }
    for (uint8_t i=0; i < len; ++i) {
        add_char_to_display_buffer(msg[i], i%4, i/4);
    }
}

void write_to_display(uint8_t display) {
    TWI_write_bytes(DISPLAY_SLAVE_ADDRESS_START + display, 0b0,
                    &display_buffer[display][0], DISPLAY_BUFFER_LEN);
}

void write_to_displays() {
    for (uint8_t i=0; i<DISPLAYS; ++i)
        write_to_display(i);
}

