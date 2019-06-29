#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "serialUtils.h"
#include "meshUtils.h"
#include "EEPROMUtils.h"

#define TIMER2_RATE 100000 // microseconds

static uint8_t pinOut[8] = {PB13, PB14, PB15, PA8, PA15, PB3, PB4, PB5};
volatile uint8_t pinOutState, pinOutState_old;
static uint8_t pinIn[8] = {PB11, PB10, PB1, PA3, PA2, PA1, PA0, PC13};
static uint8_t pinInState[8], pinState[8], pinInStateOld[8], pubFlag[8];
static unsigned long lastDebounceTime[8];
static unsigned long debounceDelay = 50;
// static uint8_t btnCheckIdx = 0;

void btn_check(uint8_t _idx) {
  pinInState[_idx] = digitalRead(pinIn[_idx]);
  if (pinInState[_idx] ^ pinInStateOld[_idx]) {
    lastDebounceTime[_idx] = millis();
  }

  if ((millis() - lastDebounceTime[_idx]) > debounceDelay) {
    if (pinInState[_idx] ^ pinState[_idx]) {
      pinState[_idx] = pinInState[_idx];
      if (pinState[_idx] == HIGH) {
        pinOutState = pinOutState ^ (1 << _idx);
        digitalWrite(pinOut[_idx], (pinOutState >> _idx) & 1);
        pubFlag[_idx] = 1;
        // sendMeshNodePublishPinVal(pinOut[_idx], digitalRead(pinOut[_idx]));
      }
    }
  }

  pinInStateOld[_idx] = pinInState[_idx];
}

void timer2_handler(void) {
  for(int i=0; i < 8; i++) {
    btn_check(i);
  }
  if (pinOutState_old ^ pinOutState) {
    EEWriteRelayState((uint16_t)pinOutState);
    pinOutState_old = pinOutState;
  }
  //if(btnCheckIdx >=8 ) btnCheckIdx = 0;
}

void timer2_begin(void) {
  Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(TIMER2_RATE); // in microseconds
  Timer2.setCompare(TIMER_CH1, 1);      // overflow might be small
  Timer2.attachInterrupt(TIMER_CH1, timer2_handler);
}

void GPIO_begin(void) {
    pinOutState = (uint8_t)EELoadRelayState();
    pinOutState_old = pinOutState;
    for(int i=0; i < 8; i++) {
        pinMode(pinOut[i], OUTPUT);
        digitalWrite(pinOut[i], (pinOutState >> i) & 1);
        pinMode(pinIn[i], INPUT_PULLDOWN);
        pubFlag[i] = 0;
    }
    timer2_begin();
}

uint8_t getPinNo(uint8_t _pin) {
  switch(_pin) {
    case PB13: return 0;
    case PB14: return 1;
    case PB15: return 2;
    case PA8: return 3;
    case PA15: return 4;
    case PB3: return 5;
    case PB4: return 6; 
    case PB5: return 7;
    default: return 8;
  }
}

void setGPIOPin(uint8_t _pin, uint8_t _val) {
  uint8_t idx = getPinNo(_pin);
  DEBUGPRINT(F("set pin: ")); DEBUGPRINTF(_pin, DEC);DEBUGPRINTLN(F(""));
  DEBUGPRINT(F("idx: ")); DEBUGPRINTF(idx, DEC);DEBUGPRINTLN(F(""));
  DEBUGPRINT(F("val: ")); DEBUGPRINTF(_val, DEC);
  DEBUGPRINTLN(F(""));
  if (getPinNo(_pin) < 8) {
    if (_val) {
      pinOutState = pinOutState | (1 << getPinNo(_pin));
    } else {
      pinOutState = pinOutState & ~(1 << getPinNo(_pin));
    }
    digitalWrite(_pin, (pinOutState >> getPinNo(_pin)) & 1);
  } else {
    digitalWrite(_pin, _val);
  }
}

void GPIO_loop(void) {
   for(int i=0; i < 8; i++) {
      if(pubFlag[i]) {
        pubFlag[i] = 0;
        DEBUGPRINT("pinInState("); DEBUGPRINTF(pinIn[i], DEC); DEBUGPRINT("):"); DEBUGPRINTLN(pinInState[i]);
        DEBUGPRINT("pinOutState("); DEBUGPRINTF(pinOut[i], DEC); DEBUGPRINT("):"); DEBUGPRINTLN(digitalRead(pinOut[i]));
        sendMeshNodePublishPinVal(pinOut[i], digitalRead(pinOut[i]));
      }
    }
}
  