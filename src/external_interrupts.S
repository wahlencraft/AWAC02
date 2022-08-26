#include <avr/io.h>
#include "constants.h"

#define INTERRUPT_MASK (1<<BUTTON_SP) | (1<<BUTTON_L) | (1<<BUTTON_R) | (1<<RTC_INT)

.data
.comm pin_memory, 1
.comm triggered_interrupts, 1
.comm irc_counter, 1
.global irc_counter

.text

.global init_external_interrupts
init_external_interrupts:
    ; Enable pin change interrupt 2
    LDS R24, PCICR;
    ORI R24, 1<<PCIE2;
    STS PCICR, R24;

    ; Set mask
    LDI R24, INTERRUPT_MASK;
    STS PCMSK2, R24;

    ; Load and save current pin status
    IN R24, PIND - 0x20;
    ANDI R24, INTERRUPT_MASK;
    STS pin_memory, R24;

    RET;

.global PCINT2_vect
; Pin change 2 interrupt routine
; Used for button input
;
; With each external interrupt, this routine is called.
; The variable triggered_interrupts will be set with the value corresponding to the
; button which was pressed. (Theoreticaly multiple buttons can be pressed at
; the same time, but that is unlikely.
PCINT2_vect:
    ; Save status flags
    PUSH R16;
    IN R16, SREG-0x20;

    PUSH R24;
    PUSH R25;

    LDS R24, irc_counter;
    INC R24;
    STS irc_counter, R24;

    ; Load current pin status
    IN R24, PIND - 0x20;
    ANDI R24, INTERRUPT_MASK;

    ; Load last pin status
    LDS R25, pin_memory;
    ; Save current pin status
    STS pin_memory, R24;

    ; Calculate pressed buttons
    ;
    ; toggled_pins = pins XOR pin_memory
    ; triggered_interrupts = NOT(pins) AND toggled_pins
    ; = NOT(pins) AND (pins XOR pin_memory)
    ; = (pin_memory OR pins) XOR pins
    OR R25, R24;
    EOR R25, R24;

    ; Save result in triggered_interrupts
    STS triggered_interrupts, R25;

    POP R25;
    POP R24;

    ; Restore status flags
    OUT SREG-0x20, R16;
    POP R16;

    RETI;

.global extract_external_interrupts
extract_external_interrupts:
    CLI;
    LDS R24, triggered_interrupts;
    STS triggered_interrupts, R1;
    SEI;
    RET;
