#include <avr/io.h>

; EEARL, EEARH: address
; EEDR, data
; EECR, control register
;   EEMPE: Master Write Enable (set to 1 before writing)
;   EEPE: Write Enable (set to 1 to write)
;   EERE: Read Enable (set to 1 to read)

; Usefull instructions
; SBI, CBI (Set/Clear Bit in I/o register
; SBIS, SBIC (Skip if Bit in I/o Register is Set/Cleared)
; IN, OUT

.global EEPROM_write
EEPROM_write:
    ; Wait for ongoing write operation to finish
    SBIC EECR-0x20, EEPE;
    RJMP EEPROM_write;

    CLI;

    ; Set the address
    OUT EEARL-0x20, R24;
    OUT EEARH-0x20, R25;

    ; Set data to write
    OUT EEDR-0x20, R22;

    ; Write
    SBI EECR-0x20, EEMPE;
    SBI EECR-0x20, EEPE;

    SEI;
    RET;

.global EEPROM_read
EEPROM_read:
    ; Wait for ongoing write operation to finish
    SBIC EECR-0x20, EEPE;
    RJMP EEPROM_read;

    CLI;

    ; Set the address
    OUT EEARL-0x20, R24;
    OUT EEARH-0x20, R25;

    ; Start EEPROM read
    SBI EECR-0x20, EERE

    ; Retrive data
    IN R24, EEDR-0x20;

    SEI;
    RET;
