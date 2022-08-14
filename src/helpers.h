#include <stdint.h>

#ifndef HELPERS_H_
#define HELPERS_H_

uint8_t bcd_to_number(uint8_t c);

uint8_t number_to_bcd(uint8_t num);

inline char number_to_ascii(uint8_t num) {
    return '0' + num;
}

void add_bcd_digits_to_string(char *str, uint8_t digits, uint8_t pos);

uint8_t circular_addition(int8_t low_bound, int8_t high_bound, int8_t v0, int8_t v1);
uint8_t bound_addition(int8_t low_bound, int8_t high_bound, int8_t v0, int8_t v1);

#endif  // HELPERS_H_
