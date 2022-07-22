#include <avr/io.h>

#define TWI_START           0x08
#define TWI_REPEATED_START  0x10
#define TWI_MT_SLA_ACK      0x18
#define TWI_MT_DATA_ACK     0x28
#define TWI_MR_SLA_ACK      0x40
#define TWI_MR_DATA_ACK     0x50
#define TWI_MR_DATA_NOT_ACK 0x58
#define TWI_STATUS_FREE 0
#define TWI_STATUS_WRITING 1
#define TWI_STATUS_READING 2
#define TWI_MAX_DATA_BYTES 32
#define TWI_MAX_STATUS_LOG_LEN 32

.data
.comm twi_sla_addr, 1
.global twi_sla_addr
.comm twi_regcom, 1
.global twi_regcom
.comm twi_data_len, 1
.global twi_data_len
.comm twi_data, TWI_MAX_DATA_BYTES
.global twi_data
.comm twi_data_ptr, 1
.global twi_data_ptr
.comm twi_status, 1
.global twi_status
.comm twi_status_log, TWI_MAX_STATUS_LOG_LEN
.global twi_status_log
.comm twi_status_log_len, 1
.global twi_status_log_len

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

    ; Set status "free"
    LDI R24, TWI_STATUS_FREE;
    STS twi_status, R24;

    RET;

TWI_debug_clear:
    STS twi_status_log_len, R1;
    RET;

; Input
; R24: Debug code
; Destroys XL, XH, R25
TWI_append_debug_code:
    LDS R25, twi_status_log_len;

    ; Write to twi_status_log[twi_status_log_len]
    LDI XL, lo8(twi_status_log);
    LDI XH, hi8(twi_status_log);
    ADD XL, R25;
    ADC XH, R1;
    ST X, R24;

    ; Increment debug_len
    INC R25;
    STS twi_status_log_len, R25;

    RET

.global TWI_wait
; Wait for twi to be free
TWI_wait:
    LDS R19, twi_status;
    CPI R19, TWI_STATUS_FREE;
    BREQ twi_wait_end;
    ; Enable sleep mode
    LDS R18, SMCR;
    SBR R18, (1<<SE);
    STS SMCR, R18;

    SLEEP;  Sleep until interrupt

    JMP TWI_wait;  Try again
twi_wait_end:
    RET;

.global TWI_write_byte
; Write one byte to slave
; Input:
; R24: Slave address
; R22: Data byte
TWI_write_byte:
    ; Wait for twi free
    CALL TWI_wait;

    CALL TWI_debug_clear;

    ; Set status "writing"
    LDI R19, TWI_STATUS_WRITING;
    STS twi_status, R19;

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

.global TWI_read_byte
; read one byte from slave
; Input:
; R24: Slave address
; R22: Read register
; Output:
; R24: Data byte
TWI_read_byte:
    ; Wait for twi free
    CALL TWI_wait;

    CALL TWI_debug_clear;

    ; Set status "reading"
    LDI R19, TWI_STATUS_READING;
    STS twi_status, R19;

    ; Calculate slave address and store it for later
    LSL R24;  Shift because the address is only 7 bit (last bit is R/W, should be 0)
    STS twi_sla_addr, R24;

    ; Store data in buffer
    STS twi_data_len, R1;  only register/command value, so length 0
    STS twi_regcom, R22;

    ; Send start condition, interrupt enable
    LDI R24, (1<<TWEN) | (1<<TWINT) | (1<<TWSTA) | (1<<TWIE);
    STS TWCR, R24;

    ; Wait for read to finish
    CALL TWI_wait;
    LDS R24, twi_data

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

    ; Wait for twi free
    CALL TWI_wait;

    CALL TWI_debug_clear;

    ; Set status "in use"
    LDI R19, TWI_STATUS_WRITING;
    STS twi_status, R19;

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

    ; Save status code to buffer
    CALL TWI_append_debug_code;

    ; Branch on status code
    CPI R24, TWI_START;
    BREQ TWI_vect_START;
    CPI R24, TWI_REPEATED_START
    BREQ TWI_vect_REPEATED_START;
    CPI R24, TWI_MT_SLA_ACK;
    BREQ TWI_vect_MT_SLA_ACK;
    CPI R24, TWI_MT_DATA_ACK;
    BREQ TWI_vect_MT_DATA_ACK;
    CPI R24, TWI_MR_SLA_ACK;
    BREQ TWI_vect_MR_SLA_ACK;
    ; CPI R24, TWI_MR_DATA_ACK TODO
    CPI R24, TWI_MR_DATA_NOT_ACK;
    BREQ TWI_vect_MR_DATA_NOT_ACK;
    ; Error. This point should not be reached
    SBI PINC - 0x20, 0

TWI_vect_START:  ; A start condition has been sent
TWI_vect_REPEATED_START:  ; A repeated start condition has been sent
    ; Put slave address in TWDR
    LDS R24, twi_sla_addr;
    STS TWDR, R24;
    ; Transmit
    LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
    JMP TWI_vect_end;

TWI_vect_MT_SLA_ACK:  ; Slave has sent ACK (Master Transmitter)
    ; Put register/command in TWDR
    LDS R24, twi_regcom;
    STS TWDR, R24;
    ; Transmit
    LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
    JMP TWI_vect_end;

TWI_vect_MR_SLA_ACK:  ; Slave has sent ACK (Master Receiver)
    ; LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWEA) | (1<<TWIE);  ; TODO read multiple bytes
    ; Read one byte, then send NACK
    LDI R24, (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
    JMP TWI_vect_end;

TWI_vect_MT_DATA_ACK:  ; Slave has sent ACK in response to data
    ; What is the twi status?
    LDS R19, twi_status;
    CPI R19, TWI_STATUS_READING;
    BREQ twi_restart_for_read;
    ; twi status is "writing", continue

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

TWI_vect_MR_DATA_NOT_ACK:  ; Data byte has been received; NOT ACK has been returned
    ; Save data to buffer
    ; TODO handle more than 1 byte
    LDS R24, TWDR;
    STS twi_data, R24;

    ; Send a stop condition

TWI_vect_stop:
    ; Set status "free"
    LDI R19, TWI_STATUS_FREE;
    STS twi_status, R19;

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

; This routine is called when the registry address is already written to the
; slave. Now the twi should enter master receiver mode.
twi_restart_for_read:
    ; Load the slave address
    LDS R24, twi_sla_addr;
    ; The slave address is already shifted left, but the lsb is 0 (write mode),
    ; change it to 1 (read mode)
    ORI R24, 1;
    ; Store modified slave addres
    STS twi_sla_addr, r24;
    ; Generate a reapeated start condition
    LDI R24, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE);
    JMP TWI_vect_end;

