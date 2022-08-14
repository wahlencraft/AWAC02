#include "helpers.h"

uint8_t bcd_to_number(uint8_t c) {
    return 10*((c & 0xff) >> 4) + (c & 0x0f);
}

uint8_t number_to_bcd(uint8_t num) {
    return ((num / 10) << 4) + (num % 10);
}

void add_bcd_digits_to_string(char *str, uint8_t digits, uint8_t pos) {
    // digits is BCD and should first be converted to ASCII
    str[pos] = '0' + (digits & 0x0f);
    str[pos - 1] = '0' + ((digits & 0x70) >> 4);
}

uint8_t circular_addition(int8_t low_bound, int8_t high_bound, int8_t v0, int8_t v1) {
    int8_t sum = v0 + v1;
    if (sum < low_bound) {
        return sum - low_bound + 1 + high_bound;
    } else if (sum > high_bound) {
        return sum - high_bound - 1 + low_bound;
    } else {
        return sum;
    }
}

uint8_t bound_addition(int8_t low_bound, int8_t high_bound, int8_t v0, int8_t v1) {
    int8_t sum = v0 + v1;
    if (sum < low_bound)
        return low_bound;
    if (sum > high_bound)
        return high_bound;
    return sum;
}
