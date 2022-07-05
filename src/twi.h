/* This is a header declaring the functions defined by assembly rutines in
 * twi.S.
 *
 * The TWI (2-wire Serial Interface, equivalent to I2C) is implemented with
 * interrupts. That means that writing is non-blocking, i.e. the main program
 * continues before the write is complete. Additional write operations is
 * handled by later interrupts.
 */

#ifndef TWI_H_
#define TWI_H_

extern void TWI_init();

/* Write one register/command byte to a slave.
 *
 * This function is non-blocking, and the main program will continue before the
 * write is finished. */
extern void TWI_write_byte(uint8_t sla_addr, uint8_t regcom);

/* Write one register/command byte + len data bytes to a slave
 *
 * This function is non-blocking, and the main program will continue before the
 * write is finished. */
extern void TWI_write_bytes(uint8_t sla_addr, uint8_t regcom, uint8_t *data, uint8_t len);

#endif  // TWI_H_

