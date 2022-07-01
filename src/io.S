#include <avr/io.h>

.global enable
enable:
    SBI DDRC - 0x20, 0
    RET

.global toggle
toggle:
    SBI PINC - 0x20, 0
    RET

