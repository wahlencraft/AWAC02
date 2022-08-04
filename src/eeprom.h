/* This is a header declaring the functions defined by assembly rutines in
 * eeprom.S.
 *
 * Theese functions allows reads and writes to non-volatile memory. So the data
 * will remain even if power is lost. */

#ifndef EEPROM_H_
#define EEPROM_H_

/* Write one byte to EEPROM.
 *
 * Note, this is very slow (typicaly 3.3 ms). */
extern void EEPROM_write(uint16_t address, uint8_t data);

/* Write one byte to EEPROM. */
extern uint8_t EEPROM_read(uint16_t address);

#endif  // EEPROM_H_
