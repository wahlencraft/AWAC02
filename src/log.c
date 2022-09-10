#include "log.h"

#ifdef LOG
#   include "flash.h"
#   include "rtc.h"
#endif

uint8_t log_levels = 0;

#ifdef LOG
    char* log_level_to_string(uint8_t lvl) {
        static char info[]    = "INFO";
        static char error[]   = KRED"ERROR";
        static char warning[] = KYEL"WARN";
        static char display[] = "DISP";
        static char rtc[] = "RTC";
        static char state[] = "STATE";
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
                break;
            case STATE:
                label = state;
                break;
            default:
                break;
        }
        return label;
    }
#endif

void init_log(uint8_t levels) {
#ifdef LOG
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


void log_time() {
#   ifdef LOG
        if (log_levels & RTC) {
            uint8_t time[7];
            RTC_get_all(time);
            char dotw[4] = "   \0";
            add_dotw_to_string(dotw, time[3], 0);
            printf("RTC time is: %s 20%02d-%02d-%02d %02d:%02d:%02d\n", dotw, time[6], time[5], time[4], time[2], time[1], time[0]);
        }
#   endif
}
