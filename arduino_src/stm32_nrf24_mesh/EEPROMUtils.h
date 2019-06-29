#ifndef __EEPROM_UTILS_H__
#define __EEPROM_UTILS_H__

uint8_t EELoadID(void);
uint16_t EELoadRelayState(void);

void EEWriteNewID(uint8_t _newID);
void EEWriteRelayState(uint16_t _rstate);

#endif // __EEPROM_UTILS_H__
