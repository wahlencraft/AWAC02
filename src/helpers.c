#include "helpers.h"

uint8_t bcd_to_number(char c) {
    return 10*((c & 0x7f) >> 4) + (c & 0x0f);
}

uint8_t number_to_bcd(uint8_t num) {
    return ((num / 10) << 4) + (num % 10);
}

