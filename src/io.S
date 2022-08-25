#include <avr/io.h>
#include "constants.h"

.global enable_user_alarm_pins
enable_user_alarm_pins:
    SBI DDRC - 0x20, USER_ALARM_PIN
    SBI DDRC - 0x20, SP_LED_PIN
    RET

.global toggle_user_alarm_pins
toggle_user_alarm_pins:
    SBI PINC - 0x20, USER_ALARM_PIN
    SBI PINC - 0x20, SP_LED_PIN
    RET

