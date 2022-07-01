.global mul16
; R25:R24:R23:R22 = R25:R24 * R23:R22
#define Z3 R25
#define Z2 R24
#define Z1 R23
#define Z0 R22
#define X1 R19
#define X0 R18
#define Y1 R27
#define Y0 R26
mul16:
    PUSH R0;
    MOVW R18, R24;
    MOVW R26, R22;
    ; Now calculate R25:R24:R23:R22 = R19:R18 * R27:R26
    ; Let R25:R24:R23:R22 = Z, R19:R18 = X, R27:R26 = Y
    ; We have Z = X*Y or
    ; Z3*2^24 + Z2*2^16 + Z1*2^8 + Z0*2^0 = (X1*2^8 + X0*2^0)(Y1*2^8 + Y0*2^0)
    ;
    ; Z0 = Lo(X0*Y0)
    ; Z1 = Hi(X0*Y0) + Lo(X1*Y0) + Lo(X0*Y1)
    ; Z2 = Lo(X1*Y1) + Hi(X1*Y0) + Hi(X0*Y1)
    ; Z3 = Hi(X1*Y1)

    ; Z1:Z0 = X0*Y0
    MUL X0, Y0; R1:R0 = X0*Y0
    MOVW Z0, R0; Z1:Z0 = R1:R0

    ; Z3:Z2 = X1*Y1
    MUL X1, Y1; R1:R0 = X1*Y1
    MOVW Z2, R0; Z3:Z2 = R1:R0

    ; Z1 = Z1 + Lo(X1*Y0)
    ; Z2 = Z2 + Hi(X1*Y0) + C
    MUL X1, Y0; R1:R0 = X1*Y0
    ADD Z1, R0; Z1 += R0
    ADC Z2, R1; Z2 += R1

    ; Z1 = Z1 + Lo(X0*X1)
    ; Z2 = Z2 + Hi(X0*X1) + C
    MUL X0, Y1; R1:R0 = X0*Y1
    ADD Z1, R0; Z1 += R0
    ADC Z2, R1; Z2 += R1

    CLR R1;
    POP R0;

    RET

.global div16
; Divide two 16 bit numbers.
; R25:R24 = R25:R24 / R23:R22
#define counter R1
#define numerH R17
#define numerL R16
#define denomH R23
#define denomL R22
#define storageL R18
#define resultH R25
#define resultL R24
div16:
    PUSH R16;
    PUSH R17;
    MOVW numerL, R24;

    CLR resultL;
    CLR resultH;
div16_shift_denom:
    ; Shift denominator left until overflow in high byte
    INC counter;
    LSL denomL;
    ROL denomH;
    BRCC div16_shift_denom;
div16_divide:
    ROR denomH;
    ROR denomL;
    MOVW storageL, numerL; Make safe copy of numerator
    SUB numerL, denomL; numerator = numerator - denomenator
    SBC numerH, denomH;
    BRCC div16_res1; If carry not set, result bit is 1
    ; Carry set, result bit is 0
    MOVW numerL, storageL; Undo subtraction
    CLC; Clear carry (we will use it in the result)
    JMP div16_result;
div16_res1:
    SEC; Set carry (we will use it in the result)
div16_result:
    ; Shift result left, add result bit
    ROL resultL;
    ROL resultH;
    ; Decrement counter, restart if not 0
    DEC counter;
    BRNE div16_divide;
    ; Return
    POP R17;
    POP R16;
    CLR R1;
    RET;

.global div32
#define numer0 R14
#define numer1 R15
#define numer2 R16
#define numer3 R17
#define denom0 R18
#define denom1 R19
#define denom2 R20
#define denom3 R21
#define result0 R22
#define result1 R23
#define result2 R24
#define result3 R25
#define storage0 R26
#define storage1 R27
#define storage2 R28
#define storage3 R29
#define counter R1
; Divide two 32 bit numbers
; R25:R24:R23:R22 = R25:R24:R23:R22 / R21:R20:R19:R18
div32:
    ; Prepare registers
    PUSH R14;
    PUSH R15;
    PUSH R16;
    PUSH R17;
    PUSH R28;
    PUSH R29;
    MOVW numer0, R22;
    MOVW numer2, R24;
    CLR result0;
    CLR result1;
    CLR result2;
    CLR result3;
div32_shift_denom:
    ; Shift denominator left until overflow in high byte
    INC counter;
    LSL denom0;
    ROL denom1;
    ROL denom2;
    ROL denom3;
    BRCC div32_shift_denom;
div32_divide:
    ROR denom3;
    ROR denom2;
    ROR denom1;
    ROR denom0;
    MOVW storage0, numer0; Make safe copy of numerator
    MOVW storage2, numer2;
    SUB numer0, denom0; numerator = numerator - denomenator
    SBC numer1, denom1;
    SBC numer2, denom2;
    SBC numer3, denom3;
    BRCC div32_res1; If carry not set, result bit is 1
    ; Carry set, result bit is 0
    MOVW numer0, storage0; Undo subtraction
    MOVW numer2, storage2;
    CLC; Clear carry (we will use it in the result)
    JMP div32_result;
div32_res1:
    SEC; Set carry (we will use it in the result)
div32_result:
    ; Shift result left, add result bit
    ROL result0;
    ROL result1;
    ROL result2;
    ROL result3;
    ; Decrement counter, restart if not 0
    DEC counter;
    BRNE div32_divide;
    ; Return
    POP R29;
    POP R28;
    POP R17;
    POP R16;
    POP R15;
    POP R14;
    CLR R1;
    RET;

.global div24
#define numer0 R14
#define numer1 R15
#define numer2 R16
#define denom0 R18
#define denom1 R19
#define denom2 R20
#define result0 R22
#define result1 R23
#define result2 R24
#define storage0 R26
#define storage1 R27
#define storage2 R28
#define counter R1
; Divide two 24 bit numbers
; R24:R23:R22 = R24:R23:R22 / R20:R19:R18
div24:
    ; Prepare registers
    PUSH R14;
    PUSH R15;
    PUSH R16;
    PUSH R28;
    MOVW numer0, R22;
    MOV numer2, R24;
    CLR result0;
    CLR result1;
    CLR result2;
div24_shift_denom:
    ; Shift denominator left until overflow in high byte
    INC counter;
    LSL denom0;
    ROL denom1;
    ROL denom2;
    BRCC div24_shift_denom;
div24_divide:
    ROR denom2;
    ROR denom1;
    ROR denom0;
    MOVW storage0, numer0; Make safe copy of numerator
    MOV storage2, numer2;
    SUB numer0, denom0; numerator = numerator - denomenator
    SBC numer1, denom1;
    SBC numer2, denom2;
    BRCC div24_res1; If carry not set, result bit is 1
    ; Carry set, result bit is 0
    MOVW numer0, storage0; Undo subtraction
    MOV numer2, storage2;
    CLC; Clear carry (we will use it in the result)
    JMP div24_result;
div24_res1:
    SEC; Set carry (we will use it in the result)
div24_result:
    ; Shift result left, add result bit
    ROL result0;
    ROL result1;
    ROL result2;
    ; Decrement counter, restart if not 0
    DEC counter;
    BRNE div24_divide;
    CLR R25;
    ; Return
    POP R28;
    POP R16;
    POP R15;
    POP R14;
    CLR R1;
    RET;

