#include <avr/io.h>
#include "constants.h"

#define BUTTON_MASK (1<<BUTTON0) | (1<<BUTTON1) | (1<<BUTTON2) | (1<<BUTTON3) | (1<<RTC_INT)

.data
.comm pin_memory, 1
.comm pressed_buttons, 1
.global pressed_buttons
.global pin_memory

.text

.global init_buttons
init_buttons:
    ; Enable pin change interrupt 2
    LDS R24, PCICR;
    ORI R24, 1<<PCIE2;
    STS PCICR, R24;

    ; Set mask
    LDI R24, BUTTON_MASK;
    STS PCMSK2, R24;

    ; Load and save current pin status
    IN R24, PIND - 0x20;
    ANDI R24, BUTTON_MASK;
    STS pin_memory, R24;

    RET;

.global PCINT2_vect
; Pin change 2 interrupt routine
; Used for button input
;
; With each button press, this routine is called.
; The variable pressed_buttons will be set with the value corresponding to the
; button which was pressed. (Theoreticaly multiple buttons can be pressed at
; the same time, but that is unlikely.
PCINT2_vect:
    ; Save status flags
    PUSH R16;
    IN R16, SREG-0x20;

    PUSH R24;
    PUSH R25;

    SBI PINC - 0x20, 0

    ; Load current pin status
    IN R24, PIND - 0x20;
    ANDI R24, BUTTON_MASK;

    ; Load last pin status
    LDS R25, pin_memory;
    ; Save current pin status
    STS pin_memory, R24;

    ; Calculate pressed buttons
    ;
    ; toggled_pins = pins XOR pin_memory
    ; pressed_buttons = NOT(pins) AND toggled_pins
    ; = NOT(pins) AND (pins XOR pin_memory)
    ; = (pin_memory OR pins) XOR pins
    OR R25, R24;
    EOR R25, R24;

    ; Save result in pressed_buttons
    STS pressed_buttons, R25;

    POP R25;
    POP R24;

    ; Restore status flags
    OUT SREG-0x20, R16;
    POP R16;

    RETI;

.global extract_pressed_buttons
extract_pressed_buttons:
    CLI;
    LDS R24, pressed_buttons;
    STS pressed_buttons, R1;
    SEI;
    RET;
