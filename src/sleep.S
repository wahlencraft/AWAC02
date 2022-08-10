#include <avr/io.h>

.global sleep_until_interrupt
sleep_until_interrupt:
    ; Enable sleep mode
    LDS R18, SMCR;
    SBR R18, (1<<SE);
    STS SMCR, R18;

    SLEEP;  Sleep until interrupt

    RET;
