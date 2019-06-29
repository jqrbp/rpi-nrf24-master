#ifndef __MQTT_UTILS_H__
#define __MQTT_UTILS_H__

void mqtt_publish_val(uint8_t _id, uint8_t _pin, int32_t _val);
void mqtt_publish_reach(uint8_t _id);
void mqtt_begin(void);
#endif 
