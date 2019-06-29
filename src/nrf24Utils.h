#ifndef NRF24_UTILS_H
#define NRF24_UTILS_H

#include <stdint.h>

void nrf24_init(void);
void sendMeshMasterCmd(char _head, uint8_t _idto, uint8_t _pinto, int32_t _pinvalto);

#endif //NRF24_UTILS_H