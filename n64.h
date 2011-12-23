#ifndef _N64_SYSTEM_H_
#define _N64_SYSTEM_H_

// Dumb toggle so we can actually fake buttons
// These used to be 0x04 which I think is B00000100, but if something breaks, change these
#define SET_N64_PIN_HIGH DDRD &= ~B00000100
#define SET_N64_PIN_LOW  DDRD |=  B00000100
#define QUERY_N64_PIN    (PIND &  B00000100)

class N64 {
  public:
    N64();
    ~N64();
    void init();
    int DATA_PIN;
};

#endif
