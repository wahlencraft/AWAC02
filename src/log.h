/* This header defines the logging functions.
 *
 * To log define the LOG constant in constants.h. If logging is not needed, do
 * not define it and the binary will be a lot smaller. Note that the init_log
 * and log functions can be used when LOG is undefined, then they do nothing
 * (you can easily toggle logging).
 *
 * Then call init_log. It takes an argument, a bitmap of log levels.
 * Add multiple loglevels like this: init_log(INFO | ERROR | WARNING). Only the
 * supplied log levels will be logged.
 *
 * Then to log, call the log function with a log level and then arguments as a
 * normal printf call. Like this for example: log(INFO, "Run some tests\n");
 */
#ifndef LOG_H_
#define LOG_H_

#include "constants.h"

#define INFO 1<<0
#define ERROR 1<<1
#define WARNING 1<<2

static uint8_t log_levels;

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#ifdef LOG
    char* log_level_to_string(uint8_t lvl) {
        static char info[]    = "INFO";
        static char error[]   = KRED"ERROR";
        static char warning[] = KYEL"WARN";
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
            default:
                break;
        }
        return label;
    }
#endif

#ifdef LOG
#   include <stdio.h>
#   include "usart.h"
    extern int USART_putchar_printf(char var, FILE *stream);
    static FILE usart_stdout = FDEV_SETUP_STREAM(USART_putchar_printf, NULL, _FDEV_SETUP_WRITE);
#   define log(lvl, fmt, args...)({if (lvl & log_levels) printf("%s "__FILE__": "fmt KNRM, log_level_to_string(lvl), ##args);})
#else
#   define log(lvl, fmt, args...)  // Do nothing
#endif

inline static void init_log(uint8_t levels) {
#ifdef LOG
    stdout = &usart_stdout;
    USART_init();
    log_levels = levels;
    printf("\nINITIATE LOG\n");
#endif
}

#endif  // LOG_H_
