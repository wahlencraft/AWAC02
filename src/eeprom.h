#ifndef EEPROM_H_
#define EEPROM_H_

extern void EEPROM_write(uint16_t address, uint8_t data);

extern uint8_t EEPROM_read(uint16_t address);

#endif  // EEPROM_H_
