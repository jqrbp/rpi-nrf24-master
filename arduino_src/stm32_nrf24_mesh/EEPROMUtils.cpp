#include <EEPROM.h>

#define ADDRID  (uint16_t)0x10
#define ADDRRELAYSTATE  ADDRID + 1

static uint16_t EEVal = 0;

uint8_t EELoadID(void) {
  EEVal = EEPROM.read(ADDRID);
  if(EEVal == 0xFFFF) return 1;
  else return (uint8_t)EEVal;
}

uint16_t EELoadRelayState(void) {
  EEVal = EEPROM.read(ADDRRELAYSTATE);
  return EEVal;
}

void EEWriteNewID(uint8_t _newID) {
  EEVal = (uint16_t)_newID;
  EEPROM.write(ADDRID, EEVal);
}

void EEWriteRelayState(uint16_t _rstate) {
  EEPROM.write(ADDRRELAYSTATE, _rstate);
}
