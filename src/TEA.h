#ifndef __TEA_H__
#define __TEA_H__

#include <stdint.h>

struct payload_t {
  char cmd;
  uint8_t id;
  uint8_t pin;
  int32_t val;
};

struct encpayload_t {
    uint32_t vi;
    uint32_t vj;
    uint32_t vk;
};

void encryptPayload(payload_t _p, encpayload_t &_encp);
void decryptPayload(encpayload_t _encp, payload_t &_p);

void TEAEncrypt (unsigned int* v);
void TEADecrypt (unsigned int* v);

#endif // __TEA_H__
