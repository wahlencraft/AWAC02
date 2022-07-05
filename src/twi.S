#include <avr/io.h>

#define TWI_START           0x08
#define TWI_REPEATED_START  0x10
#define TWI_MT_SLA_ACK      0x18
#define TWI_MT_DATA_ACK     0x28
#define TWI_MR_SLA_ACK      0x40
#define TWI_MR_DATA_ACK     0x50
#define TWI_MR_DATA_NOT_ACK 0x58

.data
.comm twi_sla_addr, 1
.global twi_sla_addr
.comm twi_regcom, 1
.global twi_regcom
.comm twi_data_len, 1
.global twi_data_len
.comm twi_data, 16
.global twi_data
.comm twi_data_ptr, 1
.global twi_data_ptr

.text

.global TWI_init
TWI_init:
    ; Set bit rate to f_clk/26
    LDI R24, 10;
    STS TWBR, R24;
    ; TWS7-3 = 1, TWPS = 0
    LDI R24, (0x1f<<TWS3) | (0<<TWPS0);
    STS TWSR, R24;
    ; Enable
    LDI R24, (1<<TWEN);
    STS TWCR, R24;

    RET;

.global TWI_write_byte
; Write one byte to slave
; Input:
; R24: Slave address
; R22: Data byte
TWI_write_byte:
    ; Calculate slave address and store it for later
    LSL R24;  Shift because the address is only 7 bit (last bit is R/W, should be 0)
    STS twi_sla_addr, R24;

    ; Store data in buffer
    STS twi_data_len, R1;  only register/command value, so length 0
    STS twi_regcom, R22;

    ; Send start condition, interrupt enable
    LDI R24, (1<<TWEN) | (1<<TWINT) | (1<<TWSTA) | (1<<TWIE);
    STS TWCR, R24;

    RET;

.global TWI_write_bytes
; Write multiple bytes to slave
; Input:
; R24: Slave address
; R22: Register/command
; R21:R20: Data pointer
; R18: Number of data bytes
TWI_write_bytes:
    PUSH XL;
    PUSH XH;
    PUSH YH;
    PUSH YL;

    ; Calculate slave address and store it for later
    LSL R24;  Shift because the address is only 7 bit (last bit is R/W, should be 0)
    STS twi_sla_addr, R24;

    STS twi_regcom, R22; save register/command
    STS twi_data_ptr, R1; data pointer = 0
    STS twi_data_len, R18;  save numver of data bytes

    ; X = data (address)
    MOV XH, R21;
    MOV XL, R20;

    ; Y = twi_data (address)
    LDI YH, hi8(twi_data);
    LDI YL, lo8(twi_data);
twi_writing_to_buffer:
    CP R18, R1;  R18 == 0?
    BREQ twi_writing_done;
    DEC R18;
    LD R23, X+;
    ST Y+, R23;
    JMP twi_writing_to_buffer;

twi_writing_done:
    ; Send start condition, interrupt enable
    LDI R24, (1<<TWEN) | (1<<TWINT) | (1<<TWSTA) | (1<<TWIE);
    STS TWCR, R24;

    POP YL;
    POP YH;
    POP XL;
    POP XH;
    RET;

;0000016c <__vector_24>:
; 16c:   1f 92           push    r1
; 16e:   0f 92           push    r0
; 170:   0f b6           in      r0, 0x3f        ; 63
; 172:   0f 92           push    r0
; 174:   11 24           eor     r1, r1
; 176:   2f 93           push    r18
; 178:   3f 93           push    r19
; 17a:   4f 93           push    r20
; 17c:   5f 93           push    r21
; 17e:   6f 93           push    r22
; 180:   7f 93           push    r23
; 182:   8f 93           push    r24
; 184:   9f 93           push    r25
; 186:   af 93           push    r26
; 188:   bf 93           push    r27
; 18a:   ef 93           push    r30
; 18c:   ff 93           push    r31
; 18e:   80 91 b9 00     lds     r24, 0x00B9     ; 0x8000b9 <__TEXT_REGION_LENGTH__+0x7e00b9>
; 192:   88 7f           andi    r24, 0xF8       ; 248
; 194:   88 31           cpi     r24, 0x18       ; 24
; 196:   01 f1           breq    .+64            ; 0x1d8 <.L14>
; 198:   88 32           cpi     r24, 0x28       ; 40
; 19a:   29 f1           breq    .+74            ; 0x1e6 <.L15>
; 19c:   88 30           cpi     r24, 0x08       ; 8
; 19e:   d9 f5           brne    .+118           ; 0x216 <.L16>
; 1a0:   8e ec           ldi     r24, 0xCE       ; 206
; 1a2:   91 e0           ldi     r25, 0x01       ; 1
; 1a4:   0e 94 1b 03     call    0x636   ; 0x636 <puts>
; 1a8:   80 91 21 02     lds     r24, 0x0221     ; 0x800221 <twi_sla_addr>
;
;000001ac <.L21>:
; 1ac:   80 93 bb 00     sts     0x00BB, r24     ; 0x8000bb <__TEXT_REGION_LENGTH__+0x7e00bb>
;
;000001b0 <.L20>:
; 1b0:   85 e8           ldi     r24, 0x85       ; 133
;
;000001b2 <.L19>:
; 1b2:   80 93 bc 00     sts     0x00BC, r24     ; 0x8000bc <__TEXT_REGION_LENGTH__+0x7e00bc>
;
;000001b6 <.L13>:
; 1b6:   ff 91           pop     r31
; 1b8:   ef 91           pop     r30
; 1ba:   bf 91           pop     r27
; 1bc:   af 91           pop     r26
; 1be:   9f 91           pop     r25
; 1c0:   8f 91           pop     r24
; 1c2:   7f 91           pop     r23
; 1c4:   6f 91           pop     r22
; 1c6:   5f 91           pop     r21
; 1c8:   4f 91           pop     r20
; 1ca:   3f 91           pop     r19
; 1cc:   2f 91           pop     r18
; 1ce:   0f 90           pop     r0
; 1d0:   0f be           out     0x3f, r0        ; 63
; 1d2:   0f 90           pop     r0
; 1d4:   1f 90           pop     r1
; 1d6:   18 95           reti

; Interupt routine for TWI
.global TWI_vect
TWI_vect:
    PUSH R16;
    PUSH R24;
    PUSH R25;
    PUSH XL;
    PUSH XH;
    IN R16, SREG-0x20;

    ; R24 = TWI status code
    LDS R24, TWSR;
    ANDI R24, 0xf8;

    ; Branch on status code
    CPI R24, TWI_START;
    BREQ TWI_vect_START;
    CPI R24, TWI_MT_SLA_ACK;
    BREQ TWI_vect_MT_SLA_ACK;
    CPI R24, TWI_MT_DATA_ACK;
    BREQ TWI_vect_MT_DATA_ACK;
    ; Error. This point should not be reached

TWI_vect_START:  ; A start condition has been sent
    ; Put slave address in TWDR
    LDS R24, twi_sla_addr;
    STS TWDR, R24;
    ; Transmit
    LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
    JMP TWI_vect_end;

TWI_vect_MT_SLA_ACK:  ; Slave has sent ACK
    ; Put register/command in TWDR
    LDS R24, twi_regcom;
    STS TWDR, R24;
    ; Transmit
    LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
    JMP TWI_vect_end;

TWI_vect_MT_DATA_ACK:  ; Slave has sent ACK in response to data
    ; Is there more data?
    LDS R24, twi_data_ptr;  R24 = twi_data_ptr
    LDS R25, twi_data_len;  R25 = twi_data_len

    CP R24, R25;
    BRSH TWI_vect_stop;  branch if data_ptr >= data_len

    ; Read twi_data[twi_data_ptr];
    LDI XH, hi8(twi_data);  X = twi_data
    LDI XL, lo8(twi_data);
    ADD XL, R24;  X = twi_data + twi_data_ptr
    ADC XH, R1;
    LD R25, X;  R25 = twi_data[twi_data_ptr]
    STS TWDR, R25;

    ; data_ptr++
    INC R24;
    STS twi_data_ptr, R24;

    ; Transmit
    LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
    JMP TWI_vect_end;

TWI_vect_stop:
    LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
TWI_vect_end:
    ; Transmit
    STS TWCR, R24;

    OUT SREG-0x20, R16;  Restore SREG value
    POP XH;
    POP XL;
    POP R25;
    POP R24;
    POP R16;
    RETI;
