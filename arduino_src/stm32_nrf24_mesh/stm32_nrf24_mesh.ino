#include "serialUtils.h"
#include "meshUtils.h"
#include "GPIOUtils.h"
void setup() {
  GPIO_begin();
  serial_begin();
  mesh_begin();
}

void loop() {
  mesh_loop();
  GPIO_loop();
  serial_loop();
}
