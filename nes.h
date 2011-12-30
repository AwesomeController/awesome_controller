#ifndef _NES_SYSTEM_H_
#define _NES_SYSTEM_H_

#include <SPI.h>
#include <Arduino.h>
#include <avr/interrupt.h>

class NES {
  public:
    NES();
    ~NES();
    void init();
    int CLATCH_PIN;
    int CLOCK_PIN;
    int DATA_PIN;
};

#endif
