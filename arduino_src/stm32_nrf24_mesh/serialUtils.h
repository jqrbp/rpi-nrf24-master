#ifndef __SERIAL_UTILS_H__
#define __SERIAL_UTILS_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
  #define DEBUGPRINT(x) Serial.print(x)
  #define DEBUGPRINTF(x, y) Serial.print(x, y)
  #define DEBUGPRINTLN(x) Serial.println(x)
#else
  #define DEBUGPRINT(x)
  #define DEBUGPRINTF(x, y)
  #define DEBUGPRINTLN(x)
#endif // SERIAL_DEBUG

void serial_loop(void);
void serial_begin(void);
void parseCommand(char* command, byte _sIdx, int* returnValues, uint8_t *valLen);

#endif // __SERIAL_UTILS_H__
