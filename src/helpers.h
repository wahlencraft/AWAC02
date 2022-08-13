#include <stdint.h>

#ifndef HELPERS_H_
#define HELPERS_H_

uint8_t bcd_to_number(char c);

uint8_t number_to_bcd(uint8_t num);

inline char number_to_ascii(uint8_t num) {
    return '0' + num;
}

void add_bcd_digits_to_string(char *str, uint8_t digits, uint8_t pos);

#endif  // HELPERS_H_
