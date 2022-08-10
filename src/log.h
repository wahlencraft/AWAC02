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
#include <stdint.h>

#define INFO 1<<0
#define ERROR 1<<1
#define WARNING 1<<2
#define DISPLAY 1<<3
#define TWI 1<<4
#define RTC 1<<5

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

char* log_level_to_string(uint8_t lvl);
extern uint8_t log_levels;

#ifdef LOG
#   include <stdio.h>
#   include "usart.h"
    extern int USART_putchar_printf(char var, FILE *stream);
#   define log(lvl, fmt, args...)({if (lvl & log_levels) printf("%s "__FILE__": "fmt KNRM, log_level_to_string(lvl), ##args);})
#else
#   define log(lvl, fmt, args...)  // Do nothing
#endif

void init_log(uint8_t levels);

void log_twi_status_codes();

#endif  // LOG_H_
