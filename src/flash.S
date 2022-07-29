.text

; Bit index to segment:
;   x     -     -     -     -     -     -
; |\|/| |\|/x |\|/| |\|/| |\|/| x\|/| |\|/|
;  - -   - -   - -   - -   - -   - -   x -
; |/|\| |/|\| |/|\x |/|\| x/|\| |/|\| |/|\|
;   -     -     -     x     -     -     -
;   0     1     2     3     4     5     6
;
;   -     -     -     -     -     -     -
; |\|/| |x|/| |\x/| |\|x| |\|/| |\|/| |\|/|
;  - x   - -   - -   - -   - -   - -   - -
; |/|\| |/|\| |/|\| |/|\| |/|x| |/x\| |x|\|
;   -     -     -     -     -     -     -
;   7     8     9     10    11    12    13
ascii_table:
     .word 0b00000000000000 // ' ' (space)
     .word 0b00001000001000 // '!'
     .word 0b00001000000010 // '"'
     .word 0b01001011001110 // '#'
     .word 0b01001011101101 // '$'
     .word 0b10010000100100 // '%'
     .word 0b00110101011001 // '&'
     .word 0b00001000000000 // '''
     .word 0b00000000111001 // '('
     .word 0b00000000001111 // ')'
     .word 0b11111100000000 // '*'
     .word 0b01001011000000 // '+'
     .word 0b10000000000000 // ','
     .word 0b00000011000000 // '-'
     .word 0b00000000000000 // '.'
     .word 0b10010000000000 // '/'
     .word 0b00000000111111 // '0'
     .word 0b00010000000110 // '1'
     .word 0b00000011011011 // '2'
     .word 0b00000011001111 // '3'
     .word 0b00000011100110 // '4'
     .word 0b00000011101101 // '5'
     .word 0b00000011111101 // '6'
     .word 0b01010000000001 // '7'
     .word 0b00000011111111 // '8'
     .word 0b00000011100111 // '9'
     .word 0b00000000000000 // ':'
     .word 0b10001000000000 // ';'
     .word 0b00110000000000 // '<'
     .word 0b00000011001000 // '='
     .word 0b01000100000000 // '>'
     .word 0b01000010000011 // '?'
     .word 0b00001010111011 // '@'
     .word 0b00000011110111 // 'A'
     .word 0b01001010001111 // 'B'
     .word 0b00000000111001 // 'C'
     .word 0b01001000001111 // 'D'
     .word 0b00000011111001 // 'E'
     .word 0b00000011110001 // 'F'
     .word 0b00000010111101 // 'G'
     .word 0b00000011110110 // 'H'
     .word 0b01001000001001 // 'I'
     .word 0b00000000011110 // 'J'
     .word 0b00110001110000 // 'K'
     .word 0b00000000111000 // 'L'
     .word 0b00010100110110 // 'M'
     .word 0b00100100110110 // 'N'
     .word 0b00000000111111 // 'O'
     .word 0b00000011110011 // 'P'
     .word 0b00100000111111 // 'Q'
     .word 0b00100011110011 // 'R'
     .word 0b00000110001101 // 'S'
     .word 0b01001000000001 // 'T'
     .word 0b00000000111110 // 'U'
     .word 0b10010000110000 // 'V'
     .word 0b10100000110110 // 'W'
     .word 0b10110100000000 // 'X'
     .word 0b01010100000000 // 'Y'
     .word 0b10010000001001 // 'Z'
     .word 0b00000000111001 // '['
     .word 0b00100100000000 // '\'
     .word 0b00000000001111 // ']'
     .word 0b10100000000000 // '^'
     .word 0b00000000001000 // '_'
     .word 0b00000100000000 // '`'
     .word 0b00000011011111 // 'a'
     .word 0b00100001111000 // 'b'
     .word 0b00000011011000 // 'c'
     .word 0b10000010001110 // 'd'
     .word 0b00000001111001 // 'e'
     .word 0b00000001110001 // 'f'
     .word 0b00000110001111 // 'g'
     .word 0b00000011110100 // 'h'
     .word 0b01000000000000 // 'i'
     .word 0b00000000001110 // 'j'
     .word 0b01111000000000 // 'k'
     .word 0b01001000000000 // 'l'
     .word 0b01000011010100 // 'm'
     .word 0b00100001010000 // 'n'
     .word 0b00000011011100 // 'o'
     .word 0b00010001110001 // 'p'
     .word 0b00100011100011 // 'q'
     .word 0b00000001010000 // 'r'
     .word 0b00000110001101 // 's'
     .word 0b00000001111000 // 't'
     .word 0b00000000011100 // 'u'
     .word 0b10000000010000 // 'v'
     .word 0b10100000010100 // 'w'
     .word 0b10110100000000 // 'x'
     .word 0b00001010001110 // 'y'
     .word 0b10010000001001 // 'z'
     .word 0b10000101001001 // '{'
     .word 0b01001000000000 // '|'
     .word 0b00110010001001 // '}'
     .word 0b00000011010010 // '~'
     .word 0b11111111111111 // Unknown character (DEL or RUBOUT)

.global ascii2seg
; R25:R24 = ascii_table[R24]
ascii2seg:
    PUSH ZH;
    PUSH ZL;

    ; Calculate offset

    ; Check that 0x20 <= R24 < 0x7f
    ; and let R24 = R24 - 0x20
    SUBI R24, 0x20;
    BRLT ascii2seg_unknown_char;
    CPI R24, 0x7f-0x20;
    BRLT ascii2seg_ok_char;
ascii2seg_unknown_char:
    LDI R24, 95;
ascii2seg_ok_char:

    ; Load start address
    LDI ZH, hi8(ascii_table);
    LDI ZL, lo8(ascii_table);

    ; Apply offset
    LSL R24;  R24 = R24*2
    ADD ZL, R24;  Z = &ascii_table + offset*2
    ADC ZH, R1;  (R1 = 0)

    ; Get LSB
    LPM R24, Z;
    ; Get MSB
    ADIW ZL, 1;
    LPM R25, Z;

    POP ZL;
    POP ZH;
    RET;


; Day number to ASCII string
day_table:
    .byte 'M'
    .byte 'O'
    .byte 'N'
    .byte ' '
    .byte 'T'
    .byte 'U'
    .byte 'E'
    .byte ' '
    .byte 'W'
    .byte 'E'
    .byte 'D'
    .byte ' '
    .byte 'T'
    .byte 'H'
    .byte 'U'
    .byte ' '
    .byte 'F'
    .byte 'R'
    .byte 'I'
    .byte ' '
    .byte 'S'
    .byte 'A'
    .byte 'T'
    .byte ' '
    .byte 'S'
    .byte 'U'
    .byte 'N'
    .byte ' '

.global day_num_2_name
; Input
; R25:R24 Target string address
; R22 Day number
; R20 Position in target string
day_num_2_name:
    PUSH ZH;
    PUSH ZL;
    PUSH XH;
    PUSH XL;

    ; R22 = (R22 - 1)*4
    DEC R22;
    LSL R22;
    LSL R22;

    ; Load table address (day_name_table + ('day number' - 1)*4
    LDI ZH, hi8(day_table);
    LDI ZL, lo8(day_table);
    ADD ZL, R22;
    ADC ZH, R1;

    ; X = target string address
    MOVW XL, R24;
    ADD XL, R20;
    ADC XH, R1;

    ; Transfer data
    LPM R24, Z+;
    ST X+, R24;
    LPM R24, Z+;
    ST X+, R24;
    LPM R24, Z+;
    ST X+, R24;

    POP XL;
    POP XH;
    POP ZL;
    POP ZH;
    RET;
