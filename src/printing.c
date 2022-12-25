#include "printing.h"

void init_printing() {
#   ifdef LOG
        stdout = &usart_stdout;
        USART_init();
#   endif
}

#ifdef LOG
    static FILE usart_stdout = FDEV_SETUP_STREAM(USART_putchar_printf, NULL, _FDEV_SETUP_WRITE);
#endif

