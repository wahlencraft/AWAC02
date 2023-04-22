#ifndef PRINTING_H_
#define PRINTING_H_

#include "constants.h"

#ifdef LOG
#   include <stdio.h>
#   include "usart.h"
#   include <avr/pgmspace.h>
    extern int USART_putchar_printf(char var, FILE *stream);
#endif

void init_printing();

#endif  // PRINTING_H_

