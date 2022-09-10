#include "user_alarm.h"
#include "constants.h"
#include "printing.h"
#include "log.h"

#include <stdbool.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define USER_ALARM 0

#ifdef RUN_TESTS
static uint16_t performed_tests = 0;
static uint16_t failed_tests = 0;
#endif

void begin_test(char *type) {
#   ifdef RUN_TESTS
    performed_tests = 0;
    failed_tests = 0;
    printf_P(PSTR(KWHT"\nBegin %s tests"KNRM"\n"), type);
#   endif
}

void end_test() {
#   ifdef RUN_TESTS
    if (failed_tests == 0) {
        printf_P(PSTR(KGRN"All tests passed (%d/%d)\n"KNRM), performed_tests, performed_tests);
    } else {
        printf_P(PSTR(KYEL"%d/%d tests passed\n"KNRM), performed_tests - failed_tests, performed_tests);
    }
#   endif
}

void _test(bool expr, char *test_name, uint16_t line) {
#   ifdef RUN_TESTS
    performed_tests++;
    if (!expr) {
        failed_tests++;
        printf_P(PSTR("  %d | Failed test: "KRED"%s"KNRM), line, test_name);
    }
#   endif
}

#define test(expr)(_test(expr, #expr"\n", __LINE__))

void run_tests() {
#   ifdef RUN_TESTS


    if (user_alarm_get_len() != 0) {
        log(WARNING, "Can't do user alarm tests, because there is stored alarms.\n");
    } else {
        begin_test("User alarm");

        user_alarm_t alarm;
        alarm.dotw = 0;
        alarm.hour = 7;
        alarm.minute = 15;
        alarm.status = true;
        test(user_alarm_add(&alarm) == 0);
        // Was the alarm successfully added?
        test(user_alarm_exists_exact(0, 7, 15));

        // Try adding the same alarm again
        test(user_alarm_add(&alarm) == 1);

        alarm.dotw = 6;
        alarm.hour = 9;
        alarm.minute = 0;
        alarm.status = true;
        user_alarm_add(&alarm);

        // Was the alarm successfully added?
        test(user_alarm_exists(4, 7, 15));
        test(user_alarm_exists(0, 7, 15));
        test(user_alarm_exists(6, 9, 0));

        // Add a third alarm, check alarm order
        // The alarms should be ordered WD MON-SUN
        alarm.dotw = 6;
        alarm.hour = 8;
        alarm.minute = 30;
        user_alarm_add(&alarm);

        user_alarm_read(0, &alarm);
        test(alarm.dotw == 0);

        user_alarm_read(1, &alarm);
        test(alarm.dotw == 6);
        test(alarm.hour == 8);

        user_alarm_read(2, &alarm);
        test(alarm.dotw == 6);
        test(alarm.hour == 9);

        // Delete one alarm
        test(!user_alarm_delete(0));
        test(user_alarm_get_len() == 2);
        test(!user_alarm_exists(4, 7, 15));
        test(user_alarm_exists(6, 9, 0));

        // Delete the other alarms
        test(!user_alarm_delete(0));
        test(!user_alarm_delete(0));
        test(user_alarm_get_len() == 0);

        // Try to delete when there is none
        test(user_alarm_delete(0));

        // Edge case: Add 1 alarm, then add an alarm before it
        alarm.dotw = 1;
        alarm.hour = 8;
        alarm.minute = 10;
        user_alarm_add(&alarm);
        alarm.minute = 0;
        user_alarm_add(&alarm);
        user_alarm_read(0, &alarm);
        test(alarm.minute == 0);
        user_alarm_read(1, &alarm);
        test(alarm.minute == 10);

        // Delete the alarms
        test(!user_alarm_delete(0));
        test(!user_alarm_delete(0));

        end_test();
    }
#   endif
}
