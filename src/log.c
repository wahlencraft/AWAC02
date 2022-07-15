#include "log.h"

uint8_t log_levels = 0;

#ifdef LOG
    static FILE usart_stdout = FDEV_SETUP_STREAM(USART_putchar_printf, NULL, _FDEV_SETUP_WRITE);
#endif

#ifdef LOG
    char* log_level_to_string(uint8_t lvl) {
        static char info[]    = "INFO";
        static char error[]   = KRED"ERROR";
        static char warning[] = KYEL"WARN";
        static char display[] = "DISP";
        char *label = info;
        switch (lvl) {
            case INFO:
                label = info;
                break;
            case ERROR:
                label = error;
                break;
            case WARNING:
                label = warning;
                break;
            case DISPLAY:
                label = display;
                break;
            default:
                break;
        }
        return label;
    }
#endif

void init_log(uint8_t levels) {
#ifdef LOG
    stdout = &usart_stdout;
    USART_init();
    log_levels = levels;
    printf("\nINITIATE LOG\n");
#endif
}
