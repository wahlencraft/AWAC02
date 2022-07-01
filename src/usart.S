#include <avr/io.h>

#define USART_BAUDRATE 9600
#define BAUD_GEN (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

.global USART_init
USART_init:
    // Set baud rate
    LDI R16, lo8(BAUD_GEN);
    LDI R17, hi8(BAUD_GEN);
    STS UBRR0H, R17;
    STS UBRR0L, R16;
    // Enable transmitter
    LDI R16, 1<<TXEN0
    STS UCSR0B, R16
    // Set frame format: 8 data bits, 1 stop bit
    LDI R16, (0<<USBS0) | (3<<UCSZ00);
    STS UCSR0C, R16
    RET

// Transmit 1 byte of data
// R24 contains data byte
.global USART_putchar
USART_putchar:
    // Wait for empty transmit buffer
    LDS R25, UCSR0A;
    SBRS R25, UDRE0;
    RJMP USART_putchar

    // Put data (R24) into buffer, sends data
    STS UDR0, R24
    RET

// Special USART putchar for use in printf
.global USART_putchar_printf
USART_putchar_printf:
    PUSH R28;
    MOV R28, R24;

    ; Replace '\n' with '\n\r'
    CPI R24, '\n';  look for newline
    BRNE .+6;  not found, skip
    LDI R24, '\r';  replace
    CALL USART_putchar;

    MOV R24, R28;  get old value back
    CALL USART_putchar;

    ; Return
    LDI R24, 0;
    LDI R25, 0;
    POP R28;
    RET;

