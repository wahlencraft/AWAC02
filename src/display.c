#include "display.h"
#include "flash.h"
#include "twi.h"
#include "constants.h"
#include "log.h"

#define DISPLAY_BUFFER_LEN 13

#define REG_SYS_SETUP 0b00100000
#define REG_DISP_SETUP 0b10000000
#define REG_DIMM_SET 0b11100000

static uint8_t display_buffer[DISPLAYS][DISPLAY_BUFFER_LEN];

void initiate_display(uint8_t display) {
    log(INFO, "Initiating display %d\n", display);
    set_display_osc(ON, display);
    set_display_brightness(0, display);
    set_display(ON, 0, display);
}

void initiate_all_displays() {
    for (int i=0; i < DISPLAYS; ++i)
        initiate_display(i);
}

void set_display_osc(uint8_t on_off, uint8_t display) {
    char on[] = "ON";
    char off[] = "OFF";
    log(DISPLAY, "Set display %d oscillator (%s)\n", display, (on_off) ? on : off);
    TWI_write_byte(DISPLAY_SLAVE_ADDRESS_START + display, REG_SYS_SETUP | on_off);
}

void set_all_display_osc(uint8_t on_off) {
    for (uint8_t i=0; i < DISPLAYS; ++i)
        set_display_osc(on_off, i);
}

void set_display(uint8_t on_off, uint8_t blink_set, uint8_t display) {
    char on[] = "ON";
    char off[] = "OFF";
    log(DISPLAY, "Set display %d (%s), blink_set=%d\n", display, (on_off) ? on : off, blink_set);
    TWI_write_byte(DISPLAY_SLAVE_ADDRESS_START + display,
                   REG_DISP_SETUP | on_off | blink_set << 1);
}

void set_all_displays(uint8_t on_off, uint8_t blink_set) {
    for (uint8_t i=0; i < DISPLAYS; ++i)
        set_display(on_off, blink_set, i);
}

void set_display_brightness(uint8_t level, uint8_t display) {
    log(DISPLAY, "Set display %d duty to %d/16\n", display, level + 1);
    TWI_write_byte(DISPLAY_SLAVE_ADDRESS_START + display, REG_DIMM_SET | level);
}

void set_all_display_brightness(uint8_t level) {
    for (uint8_t i=0; i < DISPLAYS; ++i)
        set_display_brightness(level, i);
}

void clear_display_buffer(uint8_t display) {
    log(DISPLAY, "Clear display buffer %d\n", display);
    for (uint8_t i=0; i<13; ++i)
        display_buffer[display][i] = 0;
}

void clear_all_display_buffers() {
    for (uint8_t i=0; i<DISPLAYS; ++i)
        clear_display_buffer(i);
}

void add_char_to_display_buffer(char c, uint8_t pos, uint8_t display) {
    log(DISPLAY, "Add %c to display buffer %d:%d\n", c, display, pos);
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

void add_colon_to_display_buffer(uint8_t display) {
    log(DISPLAY, "Add colon to display buffer %d\n", display);
    display_buffer[display][1] |= 1;
}

void add_decimal_to_display_buffer(uint8_t display) {
    log(DISPLAY, "Add decimal to display buffer %d\n", display);
    display_buffer[display][3] |= 1;
}

void set_display_buffer_string(char *str, uint8_t display) {
    clear_display_buffer(display);
    add_char_to_display_buffer(str[0], 0, display);
    add_char_to_display_buffer(str[1], 1, display);
    add_char_to_display_buffer(str[2], 2, display);
    add_char_to_display_buffer(str[3], 3, display);
}

void set_display_buffer_long_string(char *str, uint8_t len) {
    for (uint8_t i=0; i < (len - 1)/4 + 1; ++i) {
        clear_display_buffer(i);
    }
    for (uint8_t i=0; i < len; ++i) {
        add_char_to_display_buffer(str[i], i%4, i/4);
    }
}

void write_to_display(uint8_t display) {
    log(DISPLAY, "Write to display %d\n", display);
    TWI_write_bytes(DISPLAY_SLAVE_ADDRESS_START + display, 0b0,
                    &display_buffer[display][0], DISPLAY_BUFFER_LEN);
}

void write_to_all_displays() {
    for (uint8_t i=0; i<DISPLAYS; ++i)
        write_to_display(i);
}

void print_display_buffer(uint8_t display) {
#   ifdef LOG
    printf("Display buffer %d: ", display);
    for (uint8_t i=0; i<DISPLAY_BUFFER_LEN; ++i)
        printf("0x%x ", display_buffer[display][i]);
    printf("\n");
#   endif
}

void print_all_display_buffers() {
    for (uint8_t i=0; i<DISPLAYS; ++i)
        print_display_buffer(i);
}

