#include <avr/io.h>

#define N F_CPU/1000000

.global start_counter0
start_counter0:
    ; Reset counter
    STS TCNT0, R0; R1 is always 0

    ; Set the prescaler to 1024
    LDI R24, 0b101<<CS00;
    STS TCCR0B, R24;

    RET

.global start_counter1
start_counter1:
    ; Reset counter
    STS TCNT1H, R1; R1 is always 0
    STS TCNT1L, R1;

    ; Set the prescaler to 1024
    LDI R24, 0b101<<CS10;
    STS TCCR1B, R24;

    RET

.global read_timer0
; R24 = TCNT0
read_timer0:
    CLI; Disable interrupts

    ; Read timer
    LDS R24, TCNT0;
    ; R24 is returned as uint8_t

    SEI; Enable interrupts
    RET;

.global read_timer1
; R25:R24 = TCNT1
read_timer1:
    CLI; Disable interrupts

    ; Read timer
    LDS R24, TCNT1L;
    LDS R25, TCNT1H;
    ; R25:R24 is returned as uint16_t

    SEI; Enable interrupts
    RET;


.global timestamp_to_ms0
; Input R24 (timestamp 1), R22 (timestamp 2)
; Output R25:R24 (ms timestamp diff) (R25 probably 0)
timestamp_to_ms0:
    ; Calculate counter differance
    MOV R25, R22;
    SUB R25, R24; R25 = R25 - R24

    ; counts = T*F_CPU/1000/PRESCALER = T*N*1,000/2^10 <=>
    ; T = 2^8*counts/(N*250)
    CLR R24; By puting counts in R25 it is already multiplied by 2^8
    LDI R23, hi8(N*250);
    LDI R22, lo8(N*250);
    CALL div16; R25:R24 = R25:R24 / R23:R22

    RET;

.global timestamp_to_ms1
; Input R25:R24 (timestamp 1), R23:R22 (timestamp 2)
; Output R25:R24 (ms timestamp diff)
timestamp_to_ms1:
    ; Calculate counter differance
    SUB R22, R24; R23:R22 = R23:R22 - R25:R24
    SBC R23, R25;

    ; counts = T*F_CPU/1000/PRESCALER = T*N*1,000/2^10 <=>
    ; T = 2^8*counts/(N*250)

    ; R24:R23:R22 = (R23:R22 << 8)
    MOV R24, R23;
    MOV R23, R22;
    CLR R22;

    ; R20:R19:R18 = N*250
    LDI R18, lo8(N*250);
    LDI R19, hi8(N*250);
    CLR R20;

    CALL div24; R24:R23:R22 = R24:R23:R22 / R20:R19:R18

    ; Output R25:R24 (throw highest byte)
    MOV R25, R23;
    MOV R24, R22;

    RET;

; Calculate number of timer counts to wait
; Input R24 (ms)
; Output R23 (counts)
calc_counts_8_bits:
    ; To wait T ms the timer should count T*F_CPU/1000/PRESCALER times
    ; Assume F_CPU = N*1,000 and PRESCALER = 1024
    ; So counts = (T*(F_CPU/1,000))/1024

    ; R25:R24 = T*F_CPU/1000
    CLR R25;
    LDI R23, hi8(F_CPU/1000);
    LDI R22, lo8(F_CPU/1000);
    CALL mul16; R25:R24:R23:R22 = R25:R24 * R23:R22

    ; R25:R24:R23 = R25:R24:R23:R22 / 256

    ; R23 = Lo(R25:R24:R23 / 4) = (T*(F_CPU/1,000))/1024 = counts
    LSR R24;
    ROR R23;

    LSR R24;
    ROR R23;

    RET;

.global busy_wait_ms0
; Input R24 (ms to wait)
busy_wait_ms0:
    ; Calculate number of counts to wait, output in R23
    CALL calc_counts_8_bits;

    ; Get current counter value
    CALL read_timer0; R24 = TCNT0

    ; Calculate OCR
    ; R24 = R24 + R23
    ADD R24, R23;

    ; Set OCR
    STS OCR0A, R24;

    ; Wait for output compare match flag
    SBIS TIFR0-0x20, OCF0A; Skip if flag bit is set
    RJMP .-4; Try again

    ; Clear output compare match flag
    SBI TIFR0-0x20, OCF0A;

    RET

.global TIMER0_COMPA_vect
; Interupt timer 0
; Used by sleep_ms0: Clear OCIEA0 and disable sleep mode
TIMER0_COMPA_vect:
    PUSH R16;
    PUSH R17;
    IN R16, SREG-0x20;

    ; Clear output compare interupt enable A for timer 1
    LDS R17, TIMSK0;
    CBR R17, (1<<OCIE0A);
    STS TIMSK0, R17;

    ; Disable sleep mode
    LDS R17, SMCR;
    CBR R17, (1<<SE);
    STS SMCR, R17;

    OUT SREG-0x20, R16;  Restore SREG value
    POP R17;
    POP R16;
    RETI;

.global sleep_ms0
; Input R24 (ms to wait)
sleep_ms0:
    ; Calculate number of counts to wait, output in R23
    CALL calc_counts_8_bits;

    ; Get current counter value
    CALL read_timer0; R24 = TCNT0

    ; Calculate OCR
    ; R24 = R24 + R23
    ADD R24, R23;

    ; Set OCR
    STS OCR0A, R24;

    ; Set output compare interrupt enable A for timer 0
    LDS R18, TIMSK0;
    SBR R18, (1<<OCIE0A);
    STS TIMSK0, R18;

sleep_ms0_loop:
    ; Enable sleep mode
    LDS R18, SMCR;
    SBR R18, (1<<SE);
    STS SMCR, R18;

    SLEEP;  Sleep until interrupt

    ; Check that OCIEA0 is cleared, otherwise it was another interrupt
    LDS R18, TIMSK0;
    SBRC R18, OCIE0A;  Skip if OCIE1A clear, otherwise sleep again
    JMP sleep_ms0_loop;

    RET;

; Calculate number of timer counts to wait
; Input R25:R24 (ms)
; Output R23:R22 (counts)
calc_counts_16_bits:
    ; To wait T ms the timer should count T*F_CPU/1000/PRESCALER times
    ; Assume F_CPU = N*1,000,000 and PRESCALER = 1024
    ; So counts = (T*(F_CPU/1,000))/1024

    LDI R23, hi8(F_CPU/1000);
    LDI R22, lo8(F_CPU/1000);
    CALL mul16; R25:R24:R23:R22 = R25:R24 * R23:R22

    ; R25:R24:R23 = R25:R24:R23:R22 / 256

    ; R24:R23 = Lo16(R25:R24:R23 / 4) = (T*(F_CPU/1,000))/1024 = counts
    LSR R25;
    ROR R24;
    ROR R23;

    LSR R25;
    ROR R24;
    ROR R23;

    ; Save counts in R23:R22
    MOV R22, R23;
    MOV R23, R24;

    RET;

.global busy_wait_ms1
; Input R25:R24 (ms to wait)
busy_wait_ms1:
    ; Calculate number of counts to wait, output in R23:R22
    CALL calc_counts_16_bits;

    ; Get current counter value
    CALL read_timer1; R25:R24 = TCNT1

    ; Calculate OCR
    ; R23:R22 = R23:R22 + R25:R24
    ADD R22, R24;
    ADC R23, R25;

    ; Set OCR
    STS OCR1AH, R23;
    STS OCR1AL, R22;

    ; Wait for output compare match flag
    SBIS TIFR1-0x20, OCF1A; Skip if flag bit is set
    RJMP .-4; Try again

    ; Clear output compare match flag
    SBI TIFR1-0x20, OCF1A;

    RET

.global TIMER1_COMPA_vect
; Interupt timer 1
; Used by sleep_ms1: Clear OCIEA1 and disable sleep mode
TIMER1_COMPA_vect:
    PUSH R16;
    PUSH R17;
    IN R16, SREG-0x20;

    ; Clear output compare interupt enable A for timer 1
    LDS R17, TIMSK1;
    CBR R17, (1<<OCIE1A);
    STS TIMSK1, R17;

    ; Disable sleep mode
    LDS R17, SMCR;
    CBR R17, (1<<SE);
    STS SMCR, R17;

    OUT SREG-0x20, R16;  Restore SREG value
    POP R17;
    POP R16;
    RETI;

.global sleep_ms1
; Input R25:R24 (ms to wait)
sleep_ms1:
    ; Calculate number of counts to wait, output in R23:R22
    CALL calc_counts_16_bits;

    ; Get current counter value
    CALL read_timer1; R25:R24 = TCNT1

    ; Calculate OCR
    ; R23:R22 = R23:R22 + R25:R24
    ADD R22, R24;
    ADC R23, R25;

    ; Set OCR
    STS OCR1AH, R23;
    STS OCR1AL, R22;

    ; Set output compare interrupt enable A for timer 1
    LDS R18, TIMSK1;
    SBR R18, (1<<OCIE1A);
    STS TIMSK1, R18;

sleep_ms1_loop:
    ; Enable sleep mode
    LDS R18, SMCR;
    SBR R18, (1<<SE);
    STS SMCR, R18;

    SLEEP;  Sleep until interrupt

    ; Check that OCIEA1 is cleared, otherwise it was another interrupt
    LDS R18, TIMSK1;
    SBRC R18, OCIE1A;  Skip if OCIE1A clear, otherwise sleep again
    JMP sleep_ms1_loop;

    RET;

