#include "serialUtils.h"
#include "meshUtils.h"
#include "EEPROMUtils.h"

//command:
// set: s,<id>,<pin>,<val>
// reach: a,<id>
// blink mesh: l,<val>

#define SERIALINLIM 16
static char serialData[SERIALINLIM];
static int data[3];
static uint8_t dataLen;

void serial_begin(void) {
  Serial.begin(115200);
}

void parseCommand(char* command, byte _sIdx, int* returnValues, uint8_t *valLen)
{
  // parsing state machine
  byte i = _sIdx, j = 0, sign = 0, valid=1; 
  char c = 0;
  int temp = 0;

  while(*(command + i) != '\0')
  {
    c = *(command + i);
    if (c == '!') break;
    switch(c)
    {
      case ',':
        returnValues[j++] = sign?-temp:temp;
        sign = 0;
        temp = 0;
        break;
      case '-':
        sign = 1;
        break;
      case '\n':
        break;
      case '\r':
        break;
      default:
        if ((c > 47) && (c < 58)) {
          temp = temp * 10 + (int)c - 48;
        } else {
          valid = 0;
        }
    }
    i++;
    if (i > SERIALINLIM) {
      valid = 0;
      break;
    }
  }

  if (valid) {
    returnValues[j] = sign?-temp:temp;
    *valLen = (uint8_t)j + 1;
  } else *valLen = 0;
}

void serial_loop(void) {
  if(Serial.available() > 0)
  {
    Serial.readBytesUntil('!', serialData, 15);
    switch(serialData[0])
    {
      case 'g':
        Serial.print("id: "); Serial.print(EELoadID(),DEC);
        Serial.println("");
        break;
      case 'i':
        parseCommand(serialData, 2, data, &dataLen);
        if (dataLen == 1) {
          EEWriteNewID((uint8_t)data[0]);
          Serial.print("saving new id: "); Serial.print(data[0], HEX);
          Serial.println("");
        }
        break;
      case 's':
        parseCommand(serialData, 2, data, &dataLen);
        DEBUGPRINT("Data len:");DEBUGPRINTF(dataLen, HEX);
        DEBUGPRINT("id:");DEBUGPRINTF((uint8_t)data[0], DEC);
        DEBUGPRINT("pin:");DEBUGPRINTF((uint8_t)data[1], DEC);
        DEBUGPRINT("val:");DEBUGPRINTF((uint8_t)data[2], DEC);
        if (dataLen == 3) {
          if((uint8_t)data[0] == EELoadID()) {
            Serial.println("serial cmd set self");
            digitalWrite((uint8_t)data[1],(uint8_t)data[2]);
          } else {
            Serial.println("serial cmd send master");
            sendMeshMasterCmd('S', (uint8_t)data[0], (uint8_t)data[1], (int32_t)data[2]);
          }
          DEBUGPRINTLN("");
        }
        break;
      case 'a':
        parseCommand(serialData, 2, data, &dataLen);
        if (dataLen == 1) {
          sendMeshMasterCmd('A', (uint8_t)data[0], 0, 0);
          DEBUGPRINTLN("serial reach cmd");
        }
        break;
      case 'l':
        parseCommand(serialData, 2, data, &dataLen);
        if (dataLen == 1) {
          setMeshLedToggling(data[0]);
          DEBUGPRINTLN("serial led toggle");
        }
        break;
    }
    // always echo
    DEBUGPRINTLN(serialData);

    memset(serialData, 0, sizeof(serialData));
  }
}
