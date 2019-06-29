#ifndef __MESH_UTILS_H__
#define __MESH_UTILS_H__

void mesh_begin(void);
void mesh_loop(void);
void sendMeshMasterCmd(char _head, uint8_t _idto, uint8_t _pinto, int32_t _pinvalto);
void sendMeshNodePublishPinVal(uint8_t _pinto, int32_t _pinvalto);
void setMeshLedToggling(uint8_t _flag);

#endif // __MESH_UTILS_H__
