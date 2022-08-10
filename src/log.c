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
        static char rtc[] = "RTC";
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
            case RTC:
                label = rtc;
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

extern uint8_t twi_status_log[32];
extern uint8_t twi_status_log_len;

void log_twi_status_codes() {
#ifdef LOG
    if (log_levels & TWI) {
        // Print twi status codes from the last transaction
        printf("TWI status codes: ");
        for (int i=0; i<twi_status_log_len; ++i)
            printf("0x%x ", twi_status_log[i]);
        printf("\n");
    }
#endif
}
