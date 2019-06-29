#include <stdio.h>
#include "nrf24Utils.h"
#include "restweb.h"
#include "mqttUtils.h"
#include "configUtils.h"

int main(int argc, char** argv) {
  read_config_file(argc, argv);
  nrf24_init();
  // restweb_init(argc, argv);
  mqtt_begin();
  
  while(true)
  {
  }

  printf("Program exited!\r\n");

  return 0;
}

      
      
      
