#include "helpers.h"

uint8_t bcd_to_number(char c) {
    return 10*((c & 0x7f) >> 4) + (c & 0x0f);
}

uint8_t number_to_bcd(uint8_t num) {
    return ((num / 10) << 4) + (num % 10);
}

void add_bcd_digits_to_string(char *str, uint8_t digits, uint8_t pos) {
    // digits is BCD and should first be converted to ASCII
    str[pos] = '0' + (digits & 0x0f);
    str[pos - 1] = '0' + ((digits & 0x70) >> 4);
}

