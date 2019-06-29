#include <signal.h>
#include <iostream>
#include <RF24Mesh/RF24Mesh.h> 
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include "mqttUtils.h"
#include "TEA.h"
#include "configUtils.h"

payload_t payload;
encpayload_t encpayload;

RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
RF24Network network(radio);
RF24Mesh mesh(radio,network);

static int nrf24_run = 1;
static int nrf24_run_old = 1;

void sendMeshMasterCmd(char _head, uint8_t _idto, uint8_t _pinto, int32_t _pinvalto) {
  RF24NetworkHeader header(mesh.getAddress(_idto), 0);
  payload = {_head, _idto, _pinto, _pinvalto};
  encryptPayload(payload, encpayload);
  if(network.write(header, &encpayload, sizeof(encpayload)) == 1) 
    printf("rf send mmsg success\r\n");
  else 
    printf("ERROR: rf send mmsg failed\r\n");      
}

void master_iterate_nodes(void) {
   // Check for incoming data from the sensors
    while(network.available() && nrf24_run){
    //    printf("rcv\n");
        RF24NetworkHeader header;
        network.peek(header);
        
        uint32_t dat=0;
        switch(header.type){
        // Display the incoming millis() values from the sensor nodes
        case 'R':
                  network.read(header, &encpayload, sizeof(encpayload));
                  decryptPayload(encpayload, payload);
                  printf("Rcv R %u from 0%o ID: %d\n",dat,header.from_node,(int)payload.id);
                  if(payload.cmd == 'R') {
                    mqtt_publish_val(payload.id, payload.pin, payload.val);
                  }
                  break;
        case 'A':
                  network.read(header, &encpayload, sizeof(encpayload));
                  decryptPayload(encpayload, payload);
                  printf("Rcv A %u from 0%o ID: %d\n",dat,header.from_node,(int)payload.id);
                  if(payload.cmd == 'A') {
                    printf("publish to mqtt");
                    mqtt_publish_reach(payload.id);
                  }
                  break;
        case 'M': network.read(header,&dat,sizeof(dat)); 
                  printf("Rcv %u from 0%o\n",dat,header.from_node);
                  break;
        default:  network.read(header,0,0); 
                  printf("Rcv bad type %d from 0%o\n",header.type,header.from_node); 
                  break;
        }
    }
    delay(2);
}

void nrf24_signal_handle(int s) {
  nrf24_run = 0;
  printf("waiting nrf24 thread to exit\r\n");
  while(nrf24_run_old) delay(10);
  printf("nrf24 thread exited\r\n");
  exit(s);
}

void *nrf24_thread_func(void *arg) {
  // Call network.update as usual to keep the network updated
  while(nrf24_run) {
    if(nrf24_run) mesh.update();
    if(nrf24_run) mesh.DHCP();
    if(nrf24_run) master_iterate_nodes();
  }

  nrf24_run_old = 0;

  return NULL;
}

void nrf24_init(void) {
  const char* nrf24_datarate = (const char*)get_config_nrf24_datarate();
  pthread_t nrf24_th;

  signal(SIGINT, nrf24_signal_handle);
	signal(SIGTERM, nrf24_signal_handle);

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  // Connect to the mesh
  printf("start\n");

  if (strcmp(nrf24_datarate, "RF24_250KBPS")==0) mesh.begin((uint8_t)get_config_nrf24_channel(), RF24_250KBPS);
  else if (strcmp(nrf24_datarate, "RF24_1MBPS")==0) mesh.begin((uint8_t)get_config_nrf24_channel(), RF24_1MBPS);
  else if (strcmp(nrf24_datarate, "RF24_2MBPS")==0) mesh.begin((uint8_t)get_config_nrf24_channel(), RF24_2MBPS);
  else mesh.begin((uint8_t)get_config_nrf24_channel(), RF24_1MBPS);
  
  // mesh.begin();
  radio.printDetails();

  pthread_create(&nrf24_th,NULL,nrf24_thread_func,(void *)"starting nrf24 thread ...");
}