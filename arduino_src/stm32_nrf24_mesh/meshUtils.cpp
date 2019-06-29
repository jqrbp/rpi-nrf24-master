#include "RF24Network.h"
#include "RF24_STM32.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include "EEPROMUtils.h"
#include "serialUtils.h"
#include "GPIOUtils.h"
#include "TEA.h"

#if defined(ESP8266)
  #define PINCE 16
  #define PINCS 0
  #define PINLED  2
#else
  #define PINCE PB0
  #define PINCS PA4
  #define PINLED  PB12
#endif

#define TLED  28

uint32_t ledTime = 0;
uint8_t ledVal = 0;

RF24 radio(PINCE,PINCS);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

static uint8_t nodeIDnum  = 1;
static uint8_t meshChannel = 60; //1-127, default 97
static uint8_t togglingIdx = 0;
static uint8_t togglingVal = 0;

static uint32_t displayTimer = 0;
static uint32_t publishTimer = 0;
static uint8_t ledTogglingFlag = 0;
static bool meshOK = 0;

payload_t payload;
encpayload_t encpayload;
    
void mesh_begin(void) {
  pinMode(PINLED, OUTPUT);
  nodeIDnum = EELoadID();
  mesh.setNodeID(nodeIDnum);
  DEBUGPRINTLN("begin mesh request");
  meshOK = mesh.begin(meshChannel, RF24_1MBPS); //, RF24_1MBPS, MESH_RENEWAL_TIMEOUT);
  // mesh.begin();
}

bool sendMeshMasterCmd(char _head, uint8_t _idto, uint8_t _pinto, int32_t _pinvalto) {
  if (!meshOK) return 0;
  RF24NetworkHeader header(mesh.getAddress(_idto), OCT);
  payload_t cmdpayload = {_head, _idto, _pinto, _pinvalto};
  encpayload_t enccmdpayload;
  encryptPayload(cmdpayload, enccmdpayload);
  return network.write(header, &enccmdpayload, sizeof(enccmdpayload));
}

bool sendMeshNodeMsg(char _head, uint8_t _idto, uint8_t _pinto, int32_t _pinvalto) {
  if (!meshOK) return 0;
  payload_t cmdpayload = {_head, _idto, _pinto, _pinvalto};
  encpayload_t enccmdpayload;
  encryptPayload(cmdpayload, enccmdpayload);
  return mesh.write(&enccmdpayload, _head, sizeof(enccmdpayload));
}

void sendMeshNodePublishPinVal(uint8_t _pinto, int32_t _pinvalto) {
  sendMeshNodeMsg('R', nodeIDnum, _pinto, _pinvalto);
}


void sendMeshNodePublishAvailable(void) {
  sendMeshNodeMsg('A', nodeIDnum, 0, 0);
}

void master_loop(void) {
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();
  
  // Check for incoming data from the sensors
  if(network.available()){
    RF24NetworkHeader header;
    network.peek(header);

    uint32_t dat=0;
    switch(header.type){
      // Display the incoming millis() values from the sensor nodes
      case 'M': network.read(header,&dat,sizeof(dat)); DEBUGPRINTLN(dat); break;
      case 'R':
        network.read(header, &encpayload, sizeof(encpayload));
        decryptPayload(encpayload, payload);
        if(payload.cmd == 'R') {
          DEBUGPRINT(F("ID: ")); DEBUGPRINTF(payload.id, DEC); DEBUGPRINTLN("");
          DEBUGPRINT(F("Pin: ")); DEBUGPRINTF(payload.pin, HEX); DEBUGPRINTLN("");
          DEBUGPRINT(F("Val: ")); DEBUGPRINTF(payload.val, DEC); DEBUGPRINTLN("");
        }
        break;
      default: network.read(header,0,0); DEBUGPRINTLN(header.type);break;
    }
  }

  if(ledTogglingFlag) {
    if(millis() - ledTime > 500) {
      ledVal = !ledVal;
      digitalWrite(PINLED, ledVal);
      ledTime = millis();
      if(ledVal) {
        if(mesh.addrListTop) {
          sendMeshMasterCmd('S', (uint8_t)mesh.addrList[togglingIdx].nodeID, (uint8_t)TLED, (int32_t)togglingVal);
          togglingIdx++;
          if(togglingIdx >= mesh.addrListTop) {
            togglingIdx = 0;
            togglingVal = !togglingVal;
          }
        }
      }
    }
  }
}

bool node_loop(void) {
  // Send to the master node every second
  if (millis() - displayTimer >= 5000) {
    DEBUGPRINTLN("Sending To Master Node 5s");
    displayTimer = millis();

    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&displayTimer, 'M', sizeof(displayTimer))) {
      if ( ! mesh.checkConnection() ) {
        meshOK = 0;
        return 0;
        // DEBUGPRINTLN("Renewing Address");
        // mesh.renewAddress(2000);
      }
    } else {
      DEBUGPRINT("Send OK: "); DEBUGPRINTLN(displayTimer);
    }
  }

  if (millis() - publishTimer >= 3000000) {
    publishTimer = millis();
    sendMeshNodePublishAvailable();
  }

  if(network.available()){
    displayTimer = millis();
    RF24NetworkHeader header;
    network.read(header, &encpayload, sizeof(encpayload));
    decryptPayload(encpayload, payload);
    switch(payload.cmd) {
      case 'S':
        if(payload.id == nodeIDnum) {
          DEBUGPRINT(F("Received A from Master (set pin)\r\n"));
          if(payload.val == 1) setGPIOPin((uint8_t)payload.pin, (uint8_t)HIGH);
          else if(payload.val == 0) setGPIOPin((uint8_t)payload.pin, (uint8_t)LOW);
        } else {
          break;
        }
      case 'R':
        DEBUGPRINT(F("Received R from Master (Publish)\r\n"));
        payload.cmd = 'R';
        payload.val = (int32_t)digitalRead(payload.pin);
        sendMeshNodePublishPinVal((uint8_t)payload.pin, (int32_t)payload.val);
        break;
      case 'A':
        DEBUGPRINT(F("Received A from Master (reachability)\r\n"));
        sendMeshNodePublishAvailable();
        break;
      default: break;
    }
  }
}

void mesh_loop(void) {
  mesh.update();

  if(nodeIDnum == 0) master_loop();
  else if(meshOK) node_loop();
  else {
    if (mesh.renewAddress(1000) > 0) meshOK = 1;
  }
}

void setMeshLedToggling(uint8_t _flag) {
  ledTogglingFlag = _flag;
}
