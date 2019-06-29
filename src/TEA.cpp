#include <stdlib.h>
#include "TEA.h"

static uint32_t k[4] = {0x6f6e6e61, 0x676e6979, 0x6e6f6d20, 0x0079655b };

void encryptPayload(payload_t _p, encpayload_t &_encp) {
  char cmd = _p.cmd;
  uint8_t id = _p.id;
  uint8_t pin = _p.pin;
  int32_t val = _p.val;

  uint32_t v0=((cmd << 16) | (id << 8) | (pin)), v1=(uint32_t) val, sum=0, i;           /* set up */
  uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
  uint32_t kr0 = (uint32_t)rand();
  uint32_t k0=(k[0] ^ kr0), k1=k[1], k2=k[2], k3=k[3];   /* cache key */
  for (i=0; i < 32; i++) {                       /* basic cycle start */
    sum += delta;
    v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
    v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);  
  }                                              /* end cycle */
  
  _encp = {v0, v1, k0};
}

void decryptPayload(encpayload_t _encp, payload_t &_p) {
    char cmd = 0;
    uint8_t id = 0;
    uint8_t pin = 0;
    int32_t val = 0;
    uint32_t v0=_encp.vi, v1=_encp.vj, sum=0xC6EF3720, i;  /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=_encp.vk, k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;                                   
    }                                              /* end cycle */

    cmd = ((v0 & 0x00ff0000) >> 16);
    id = ((v0 & 0x0000ff00) >> 8);
    pin = (v0 & 0x000000ff);
    val = (int32_t) v1;
    _p = {cmd, id, pin, val};
}

void TEAEncrypt (uint32_t* v) {
    uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i < 32; i++) {                       /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);  
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

void TEADecrypt (uint32_t* v) {
    uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;                                   
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}
