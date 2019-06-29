#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf24Utils.h"
#include "configUtils.h"

using namespace std;

#define SERIALINLIM 16

void parseCommand(char* command, uint8_t _sIdx, int* returnValues, uint8_t *valLen)
{
  // parsing state machine
  uint8_t i = _sIdx, j = 0, sign = 0, valid=1; 
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

bool parser_check_id(const char *_payld, const char *_id, unsigned int len) {
  bool outFlag = true;
  if(len > strlen(_id)) {
    for (unsigned int i = 0; i < strlen(_id); i++) {
        if (_payld[i] ^ _id[i]) {
            printf("ERROR: post id is not match i= %d, val=%c vs %c!\r\n", i, _payld[i], _id[i]);
            outFlag = false;
            break;
        }
    }
  } else {
      printf("ERROR: payload too small!\r\n");
      outFlag = false;
  }

  return outFlag;
}

void exec_http_postbody(const char *paychr) {
    int data[3];
    uint8_t dataLen;
    const char *wrf_id = (const char*)get_config_wrf_id();
    
    if (parser_check_id(paychr, wrf_id, strlen(paychr))) {
        parseCommand((char*)paychr, strlen(wrf_id) + 1, data, &dataLen);
        if (dataLen == 3) {
            sendMeshMasterCmd('S', (uint8_t)data[0], (uint8_t)data[1], (uint8_t)data[2]);
        }
        printf("id: 0x%02X, pin: 0x%02X, val: 0x%02X\r\n", data[0], data[1], data[2]);
    }
}