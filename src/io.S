#include <avr/io.h>
#include "constants.h"

.global user_alarm_pin_enable
user_alarm_pin_enable:
    SBI DDRC - 0x20, USER_ALARM_PIN
    RET

.global user_alarm_pin_toggle
user_alarm_pin_toggle:
    SBI PINC - 0x20, USER_ALARM_PIN
    RET

.global user_alarm_pin_off
user_alarm_pin_off:
    CBI PORTC - 0x20, USER_ALARM_PIN
    RET
